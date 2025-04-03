#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>

// Define DAG structure with parent tracking
struct DAG {
    std::vector<std::string> vertices;
    std::vector<std::pair<std::string, std::string>> edges;
    std::map<std::string, std::vector<std::string>> parents; // For quick parent lookup
};

// Define PetriNet structure
struct PetriNet {
    std::vector<std::string> places;
    std::map<std::string, std::string> place_types;
    std::vector<std::string> transitions;
    std::map<std::string, std::vector<std::string>> transition_inputs;
    std::map<std::string, std::vector<std::string>> transition_outputs;
};

// Generate DAG with "T_" prefix and populate parents
DAG generate_dag(const std::vector<std::string>& raw_vertices, const std::vector<std::pair<std::string, std::string>>& raw_edges) {
    DAG dag;
    for (const auto& v : raw_vertices) {
        dag.vertices.push_back("T_" + v);
    }
    for (const auto& edge : raw_edges) {
        std::string from = "T_" + edge.first;
        std::string to = "T_" + edge.second;
        dag.edges.emplace_back(from, to);
        dag.parents[to].push_back(from);
    }
    return dag;
}

// Find the root node
std::string find_root_node(const DAG& dag) {
    std::set<std::string> root_nodes(dag.vertices.begin(), dag.vertices.end());
    for (const auto& edge : dag.edges) {
        root_nodes.erase(edge.second);
    }
    return root_nodes.empty() ? "" : *root_nodes.begin();
}
PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;

    // Add all transitions, including T_0 and T_end
    net.transitions = dag.vertices;           // DAG transitions (e.g., T_11, T_21)
    net.transitions.push_back("T_0");         // Initial transition
    net.transitions.push_back("T_end");       // Final transition

    // Define global places (read-only)
    std::vector<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};
    for (const auto& g : globals) {
        net.places.push_back(g);
        net.place_types[g] = "string";
    }

    // Add "data" place globally (written by T_0, read by others)
    net.places.push_back("data");
    net.place_types["data"] = "feynman";

    // Add places and connections for DAG transitions
    for (const auto& transition : dag.vertices) {
        std::string ctrl = "control_" + transition.substr(2);  // e.g., control_11 for T_11
        std::string data_out = "data_" + transition.substr(2); // e.g., data_11 for T_11

        net.places.push_back(ctrl);
        net.place_types[ctrl] = "control";
        net.places.push_back(data_out);
        net.place_types[data_out] = "feynman";

        // Inputs: control (consumed), globals (read-only), "data" (read-only), parent data places
        net.transition_inputs[transition] = {ctrl, "data"};
        net.transition_inputs[transition].insert(net.transition_inputs[transition].end(), globals.begin(), globals.end());
        if (dag.parents.find(transition) != dag.parents.end()) {
            for (const auto& parent : dag.parents.at(transition)) {
                net.transition_inputs[transition].push_back("data_" + parent.substr(2));
            }
        }

        // Outputs: specific data_out only (no "data")
        net.transition_outputs[transition] = {data_out};
    }

    // Set up T_0 (initial transition, outputs "data")
    net.places.push_back("control_T_0");
    net.place_types["control_T_0"] = "control";
    net.transition_inputs["T_0"] = {"control_T_0", "library_name", "base_filename", "input"};
    net.transition_outputs["T_0"] = {"labeledgraph", "data"};

    // Set up T_end (final transition)
    net.places.push_back("control_T_end");
    net.place_types["control_T_end"] = "control";
    net.places.push_back("final_data");
    net.place_types["final_data"] = "feynman";
    net.transition_inputs["T_end"] = {"control_T_end"};
    for (const auto& transition : dag.vertices) {
        bool is_leaf = true;
        for (const auto& edge : dag.edges) {
            if (edge.first == transition) {
                is_leaf = false;
                break;
            }
        }
        if (is_leaf) {
            net.transition_inputs["T_end"].push_back("data_" + transition.substr(2));
        }
    }
    net.transition_outputs["T_end"] = {"final_data"};

    return net;
}

std::string generateXPNetXMLWithT0Tend(const PetriNet& net) {
    std::ostringstream xml;
    std::set<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};

    // XML header and struct definition
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<defun name=\"template\">\n";
    xml << "    <struct name=\"feynman\">\n";
    xml << "        <field name=\"MI\" type=\"string\"/>\n";
    xml << "        <field name=\"reducedIBPs\" type=\"string\"/>\n";
    xml << "        <field name=\"web\" type=\"string\"/>\n";
    xml << "    </struct>\n\n";

    // Top-level inputs and outputs
    xml << "    <in name=\"base_filename\" type=\"string\" place=\"base_filename\"/>\n";
    xml << "    <in name=\"library_name\" type=\"string\" place=\"library_name\"/>\n";
    xml << "    <in name=\"input\" type=\"string\" place=\"input\"/>\n";
    xml << "    <out name=\"output\" type=\"feynman\" place=\"final_data\"/>\n\n";
    xml << "    <net>\n";

    // Declare places
    for (const auto& place : net.places) {
        auto it = net.place_types.find(place);
        if (it != net.place_types.end()) {
            std::string type = it->second;
            xml << "        <place name=\"" << place << "\" type=\"" << type << "\">\n";
            if (type == "control") {
                xml << "            <token><value>[]</value></token>\n";
            }
            xml << "        </place>\n";
        } else {
            std::cerr << "Warning: Place '" << place << "' not found in place_types!\n";
        }
    }

    // Declare transitions
    for (const auto& transition : net.transitions) {
        xml << "        <transition name=\"" << transition << "\">\n";
        xml << "            <defun>\n";
        xml << "                <require key=\"worker\" mandatory=\"true\"/>\n";

        // Identify control, global, and data ports
        std::string control_input;
        std::vector<std::string> global_inputs;
        std::vector<std::string> data_inputs, data_outputs;
        std::set<std::string> inputs_set;

        if (net.transition_inputs.count(transition)) {
            for (const auto& input : net.transition_inputs.at(transition)) {
                auto it = net.place_types.find(input);
                if (it != net.place_types.end()) {
                    if (it->second == "control") {
                        control_input = input;
                    } else if (globals.count(input)) {
                        global_inputs.push_back(input);
                    } else if (input == "data" && transition != "T_0") {
                        global_inputs.push_back(input); // "data" is read-only except for T_0
                    } else {
                        inputs_set.insert(input);
                        data_inputs.push_back(input);
                    }
                }
            }
        }
        if (net.transition_outputs.count(transition)) {
            for (const auto& output : net.transition_outputs.at(transition)) {
                data_outputs.push_back(output);
            }
        }

        // Define ports
        for (const auto& input : data_inputs) {
            xml << "                <in name=\"" << input << "\" type=\"" << net.place_types.at(input) << "\"/>\n";
        }
        for (const auto& output : data_outputs) {
            xml << "                <out name=\"" << output << "\" type=\"" << net.place_types.at(output) << "\"/>\n";
        }
        for (const auto& global : global_inputs) {
            xml << "                <in name=\"" << global << "\" type=\"" << (global == "data" ? "feynman" : "string") << "\"/>\n";
        }
        if (!control_input.empty()) {
            xml << "                <in name=\"" << control_input << "\" type=\"control\"/>\n";
        }

        // Function arguments
        std::vector<std::string> function_args;
        if (!control_input.empty()) function_args.push_back(control_input);
        function_args.insert(function_args.end(), global_inputs.begin(), global_inputs.end());
        function_args.insert(function_args.end(), data_inputs.begin(), data_inputs.end());
        function_args.insert(function_args.end(), data_outputs.begin(), data_outputs.end());

        // Module definition
        xml << "                <module name=\"singular_template\" require_function_unloads_without_rest=\"false\" function=\"f_" << transition << "(";
        for (size_t i = 0; i < function_args.size(); ++i) {
            xml << function_args[i] << (i < function_args.size() - 1 ? ", " : "");
        }
        xml << ")\">\n";
        xml << "                    <cinclude href=\"interface/template_interface.hpp\"/>\n";
        xml << "                    <cinclude href=\"util-generic/dynamic_linking.hpp\"/>\n";
        xml << "                    <code><![CDATA[\n";

        // Transition-specific code
        if (transition == "T_0") {
            xml << "                        labeledgraph = RESOLVE_INTERFACE_FUNCTION(singular_getReducedIBPSystem_gpi)(input, library_name, base_filename);\n";
            xml << "                        data.web = RESOLVE_INTERFACE_FUNCTION(singular_updateWeb_gpi)(input, library_name, base_filename);\n";
            xml << "                        data.reducedIBPs = RESOLVE_INTERFACE_FUNCTION(singular_reducedIBPs_gpi)(input, library_name, base_filename);\n";
            xml << "                        data.MI = RESOLVE_INTERFACE_FUNCTION(singular_MI_gpi)(input, library_name, base_filename);\n";
            xml << "                        std::cout << \"web: \" << data.web << std::endl;\n";
        } else if (transition == "T_end") {
            xml << "                        final_data.web = \"\";\n";
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                xml << "                        final_data.web += " << data_inputs[i] << ".web;\n";
            }
            if (!data_inputs.empty()) {
                xml << "                        final_data.reducedIBPs = " << data_inputs[0] << ".reducedIBPs;\n";
                xml << "                        final_data.MI = " << data_inputs[0] << ".MI;\n";
            }
        } else {
            int j = 0, k = 0;
            if (transition.size() >= 4 && transition[0] == 'T' && transition[1] == '_' &&
                std::isdigit(transition[2]) && std::isdigit(transition[3])) {
                j = transition[2] - '0';
                k = transition[3] - '0';
            } else {
                std::cerr << "Warning: Unexpected transition format: " << transition << std::endl;
            }

            // Create a list of data inputs for mergeFeynman_gpi
            xml << "                        std::list<std::string> data_list;\n";
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                xml << "                        data_list.push_back(" << data_inputs[i] << ");\n";
            }

            // Call mergeFeynman_gpi with the list
            xml << "                        std::string merged_data = RESOLVE_INTERFACE_FUNCTION(singular_mergeFeynman_gpi)(data_list, library_name, base_filename);\n";
            xml << "                        data = merged_data;\n";

            // Seed computation
            xml << "                        std::string reducedIBPs_seed = RESOLVE_INTERFACE_FUNCTION(singular_seed_gpi)(data.reducedIBPs, library_name, base_filename);\n";
            xml << "                        std::cout << \"reducedIBPs_seed: \" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(reducedIBPs_seed, library_name) << std::endl;\n";

            // Target integrals
            xml << "                        std::string targetInt = RESOLVE_INTERFACE_FUNCTION(singular_targetInt_gpi)(input, library_name, base_filename);\n";
            xml << "                        std::cout << \"Target Integrals: \" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(targetInt, library_name) << std::endl;\n";

            // Explicitly include j and k
            xml << "                        int j = " << j << ", k = " << k << ";\n";

            // Web replacement and target integrals
            xml << "                        std::string wjk = RESOLVE_INTERFACE_FUNCTION(singular_replace_two)(data.web, j, k, library_name, base_filename);\n";
            xml << "                        std::string wjk_targetInt = RESOLVE_INTERFACE_FUNCTION(singular_targetInts_gpi)(wjk, library_name, base_filename);\n";
            xml << "                        std::cout << \"web[\" << j << \",\" << k << \"].TARGET = \" << std::endl;\n";
            xml << "                        int size_wjk_targetInt = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(wjk_targetInt, library_name, base_filename);\n";
            xml << "                        std::cout << \"size_wjk_targetInt = \" << size_wjk_targetInt << std::endl;\n";

            // Update seed and compute IBP if size >= 1
            xml << "                        reducedIBPs_seed = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(targetInt, library_name, base_filename);\n";
            xml << "                        std::cout << \"Updated reducedIBPs_seed: \" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(reducedIBPs_seed, library_name) << std::endl;\n";
            xml << "                        if (size_wjk_targetInt >= 1) {\n";
            xml << "                            std::string totalIBP = RESOLVE_INTERFACE_FUNCTION(singular_computeManyIBPjk_gpi)(labeledgraph, wjk_targetInt, library_name, base_filename);\n";
            xml << "                            std::string totalIBP_IBP = RESOLVE_INTERFACE_FUNCTION(singular_IBP_gpi)(totalIBP, library_name, base_filename);\n";
            xml << "                            int size_totalIBP_ibp = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(totalIBP_IBP, library_name, base_filename);\n";
            xml << "                            std::cout << \"Size of totalIBP_IBP: \" << size_totalIBP_ibp << std::endl;\n";

            if (transition == "T_11") {
                xml << "                            std::string reducedibp_over = RESOLVE_INTERFACE_FUNCTION(singular_over_gpi)(data.reducedIBPs, library_name, base_filename);\n";
                xml << "                            std::string totalIBP_over = RESOLVE_INTERFACE_FUNCTION(singular_over_gpi)(totalIBP, library_name, base_filename);\n";
                xml << "                            reducedibp_over = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(totalIBP_over, library_name, base_filename);\n";
                xml << "                            std::cout << \"Updated reducedibp_over: \" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(reducedibp_over, library_name) << std::endl;\n";
            }

            xml << "                            std::string L = RESOLVE_INTERFACE_FUNCTION(singular_getRedIBPs_gpi)(totalIBP, 101, library_name, base_filename);\n";
            xml << "                            std::string indpndIBP = RESOLVE_INTERFACE_FUNCTION(singular_indpndIBP_gpi)(L, library_name, base_filename);\n";
            xml << "                            std::string masterAndTailIntgrals = RESOLVE_INTERFACE_FUNCTION(singular_masterAndTailIntgrals_gpi)(L, library_name, base_filename);\n";
            xml << "                            std::cout << \"size of indpndIBP: \" << RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(indpndIBP, library_name, base_filename) << std::endl;\n";
            xml << "                            std::cout << \"size of Master and Tail Integrals: \" << RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(masterAndTailIntgrals, library_name, base_filename) << std::endl;\n";

            xml << "                            std::string reducedIBPs_IBP = RESOLVE_INTERFACE_FUNCTION(singular_IBP_gpi)(data.reducedIBPs, library_name, base_filename);\n";
            xml << "                            int size_reducedIBPs_ibp = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(reducedIBPs_IBP, library_name, base_filename);\n";
            xml << "                            if (size_reducedIBPs_ibp == 0) {\n";
            xml << "                                reducedIBPs_IBP = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(indpndIBP, library_name, base_filename);\n";
            xml << "                                std::cout << \"Assigned indpndIBP to reducedIBPs_IBP.\" << std::endl;\n";
            xml << "                            } else {\n";
            xml << "                                int indpndIBP_size = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(indpndIBP, library_name, base_filename);\n";
            xml << "                                for (int i = 1; i <= indpndIBP_size; ++i) {\n";
            xml << "                                    std::string indpndIBP_i = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(indpndIBP, i, library_name, base_filename);\n";
            xml << "                                    reducedIBPs_IBP = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(indpndIBP_i, library_name, base_filename);\n";
            xml << "                                    std::cout << \"Assigned independent IBP at index \" << i << std::endl;\n";
            xml << "                                }\n";
            xml << "                            }\n";

            xml << "                            std::string w = RESOLVE_INTERFACE_FUNCTION(singular_updateWeb_later)(data.web, masterAndTailIntgrals, j, k, library_name, base_filename);\n";
            xml << "                            std::string w1 = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(w, 1, library_name, base_filename);\n";
            xml << "                            RESOLVE_INTERFACE_FUNCTION(singular_equal_gpi)(data.web, w1, library_name, base_filename);\n";
            xml << "                            std::string masterIntgralsInSector = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(w, 2, library_name, base_filename);\n";
            xml << "                            int masterIntgralsInSector_size = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(masterIntgralsInSector, library_name, base_filename);\n";
            xml << "                            std::cout << \"masterIntgralsInSector_size: \" << masterIntgralsInSector_size << std::endl;\n";

            xml << "                            int MI_size = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(data.MI, library_name, base_filename);\n";
            xml << "                            if (MI_size == 0) {\n";
            xml << "                                RESOLVE_INTERFACE_FUNCTION(singular_equal_gpi)(data.MI, masterIntgralsInSector, library_name, base_filename);\n";
            xml << "                                std::cout << \"Assigned master integrals to MI.\" << std::endl;\n";
            xml << "                            } else {\n";
            xml << "                                for (int i = 1; i <= masterIntgralsInSector_size; ++i) {\n";
            xml << "                                    std::string masterIntgralsInSector_i = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(masterIntgralsInSector, i, library_name, base_filename);\n";
            xml << "                                    int alreadyIntheList = RESOLVE_INTERFACE_FUNCTION(singular_alreadyIntheList_gpi)(data.MI, masterIntgralsInSector_i, library_name);\n";
            xml << "                                    if (!alreadyIntheList) {\n";
            xml << "                                        data.MI = RESOLVE_INTERFACE_FUNCTION(singular_equal_gpi)(data.MI, masterIntgralsInSector_i, library_name, base_filename);\n";
            xml << "                                        std::cout << \"Added master integral from sector index \" << i << std::endl;\n";
            xml << "                                    }\n";
            xml << "                                }\n";
            xml << "                            }\n";
            xml << "                        }\n";
        }

        xml << "                    ]]></code>\n";
        xml << "                </module>\n";
        xml << "            </defun>\n";

        // Connections
        if (!control_input.empty()) {
            xml << "            <connect-in port=\"" << control_input << "\" place=\"" << control_input << "\"/>\n";
        }
        for (const auto& input : data_inputs) {
            xml << "            <connect-in port=\"" << input << "\" place=\"" << input << "\"/>\n";
        }
        for (const auto& global : global_inputs) {
            if (global == "data" && transition == "T_0") {
                xml << "            <connect-out port=\"data\" place=\"data\"/>\n";
            } else {
                xml << "            <connect-read port=\"" << global << "\" place=\"" << global << "\"/>\n";
            }
        }
        for (const auto& output : data_outputs) {
            xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
        }

        xml << "        </transition>\n";
    }

    xml << "    </net>\n";
    xml << "</defun>\n";
    return xml.str();
}

// Print Petri net details
void print_petri_net(const PetriNet& net) {
    std::cout << "\nGenerated PetriNet:\n";
    std::cout << "Places:\n";
    for (const auto& place : net.places) {
        std::string type = net.place_types.count(place) ? net.place_types.at(place) : "unknown";
        std::cout << "  " << place << " (" << type << ")\n";
    }
    std::cout << "\nTransitions:\n";
    for (const auto& t : net.transitions) std::cout << "  " << t << "\n";
    std::cout << "\nTransition Inputs:\n";
    for (const auto& [t, inputs] : net.transition_inputs) {
        std::cout << "  " << t << " <- { ";
        for (const auto& input : inputs) std::cout << input << " ";
        std::cout << "}\n";
    }
    std::cout << "\nTransition Outputs:\n";
    for (const auto& [t, outputs] : net.transition_outputs) {
        std::cout << "  " << t << " -> { ";
        for (const auto& output : outputs) std::cout << output << " ";
        std::cout << "}\n";
    }
}

// Main function
int main() {
    /* std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "41"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"22", "31"}, {"22", "32"},
        {"23", "32"}, {"31", "41"}, {"32", "41"}
    }; */
  /*   std::vector<std::string> vertices = {"11", "21", "22", "23", "24", "31", "32"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"11", "24"}, {"21", "31"}, {"22", "32"},
        {"23", "31"}, {"24", "32"}
    }; */

/*     std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "33", "34","35","41","42"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"21", "33"}, {"22", "32"},{"22", "33"}, {"22", "34"}, {"23", "34"},{"23", "35"}, 
        {"31", "41"}, {"33", "41"},  {"33", "42"}, {"34", "42"}, {"35", "42"}
    }; */


    std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "33"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"21", "32"}, {"22", "32"}, {"22", "33"}, {"23", "31"}, {"23", "33"} 
    };

    DAG myDAG = generate_dag(vertices, edges);
    std::string root_node = find_root_node(myDAG);
    std::cout << "Root Node: " << root_node << "\n";

    PetriNet myPetriNet = generate_petri_net(myDAG);
   // print_petri_net(myPetriNet);

    std::string xml = generateXPNetXMLWithT0Tend(myPetriNet);
    std::ofstream file("workflow/template.xpnet");
    file << xml;
    file.close();

    return 0;
}
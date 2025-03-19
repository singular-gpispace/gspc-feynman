#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>
// Define DAG structure
struct DAG {
    std::vector<std::string> vertices;
    std::vector<std::pair<std::string, std::string>> edges;
};

struct PetriNet {
    std::vector<std::string> places;
    std::map<std::string, std::string> place_types;
    std::vector<std::string> transitions;
    std::map<std::string, std::vector<std::string>> transition_inputs;
    std::map<std::string, std::vector<std::string>> transition_outputs; // Ensure outputs are stored as a vector
};


// Function to generate DAG with "T_" prefix
DAG generate_dag(const std::vector<std::string>& raw_vertices, const std::vector<std::pair<std::string, std::string>>& raw_edges) {
    DAG dag;
    for (const auto& v : raw_vertices) {
        dag.vertices.push_back("T_" + v);
    }
    for (const auto& edge : raw_edges) {
        dag.edges.emplace_back("T_" + edge.first, "T_" + edge.second);
    }
    return dag;
}

// Function to find the root node(s)
std::string find_root_node(const DAG& dag) {
    std::set<std::string> root_nodes(dag.vertices.begin(), dag.vertices.end());
    for (const auto& edge : dag.edges) {
        root_nodes.erase(edge.second);
    }
    return root_nodes.empty() ? "" : *root_nodes.begin();
}
PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;
    net.transitions = dag.vertices;
    std::string root_node = find_root_node(dag);

    // Define global places
    std::vector<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};
    for (const auto& g : globals) {
        net.places.push_back(g);
        net.place_types[g] = "string";
    }

    // Add control and data places for each transition
    for (const auto& transition : dag.vertices) {
        std::string ctrl = "control_" + transition.substr(2);
        std::string data_out = "data_" + transition.substr(2);

        net.places.push_back(ctrl);
        net.place_types[ctrl] = "control";

        net.places.push_back(data_out);
        net.place_types[data_out] = "feynman";

        // Set default inputs: control + globals
        net.transition_inputs[transition] = {ctrl};
        net.transition_inputs[transition].insert(
            net.transition_inputs[transition].end(), globals.begin(), globals.end()
        );

        // Set output: data_out
        net.transition_outputs[transition] = {data_out};
    }

    // Connect transitions based on DAG edges
    for (const auto& edge : dag.edges) {
        const std::string& parent = edge.first;
        const std::string& child = edge.second;
        if (!net.transition_outputs[parent].empty()) {
            net.transition_inputs[child].push_back(net.transition_outputs[parent][0]);
        }
    }

    // Add T_0 transition
    net.transitions.push_back("T_0");
    net.transition_inputs["T_0"] = {"control_T_0", "library_name", "base_filename", "input"};
    net.transition_outputs["T_0"] = {"labeledgraph", "data"};

    // Add places for T_0
    net.places.push_back("control_T_0");
    net.place_types["control_T_0"] = "control";
    net.places.push_back("data");
    net.place_types["data"] = "feynman";

    // Add T_end transition
    net.transitions.push_back("T_end");
    net.transition_inputs["T_end"] = {"control_T_end"};

    // Identify unused data_xx places for T_end inputs
    std::set<std::string> data_places;
    std::set<std::string> used_as_input;

    // Collect all data_xx places from outputs
    for (const auto& [transition, outputs] : net.transition_outputs) {
        for (const auto& output : outputs) {
            if (output.rfind("data_", 0) == 0) {
                data_places.insert(output);
            }
        }
    }

    // Collect all data_xx places used as inputs
    for (const auto& [transition, inputs] : net.transition_inputs) {
        for (const auto& input : inputs) {
            if (input.rfind("data_", 0) == 0) {
                used_as_input.insert(input);
            }
        }
    }

    // Add unused data_xx places to T_end inputs
    for (const auto& data_place : data_places) {
        if (!used_as_input.count(data_place)) {
            net.transition_inputs["T_end"].push_back(data_place);
        }
    }

    // Set T_end output and places
    net.transition_outputs["T_end"] = {"final_data"};
    net.places.push_back("control_T_end");
    net.place_types["control_T_end"] = "control";
    net.places.push_back("final_data");
    net.place_types["final_data"] = "feynman";

    return net;
}


std::string generateXPNetXMLWithT0Tend(const PetriNet& net) {
    std::ostringstream xml;
    std::set<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};

    // XML header and structure definition
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<defun name=\"template\">\n";
    xml << "    <struct name=\"feynman\">\n";
    xml << "        <field name=\"MI\" type=\"string\"/>\n";
    xml << "        <field name=\"reducedIBPs\" type=\"string\"/>\n";
    xml << "        <field name=\"web\" type=\"string\"/>\n";
    xml << "    </struct>\n\n";

    // Top-level inputs and output
    xml << "    <in name=\"base_filename\" type=\"string\" place=\"base_filename\"/>\n";
    xml << "    <in name=\"library_name\" type=\"string\" place=\"library_name\"/>\n";
    xml << "    <in name=\"input\" type=\"string\" place=\"input\"/>\n";
    xml << "    <out name=\"output\" type=\"feynman\" place=\"final_data\"/>\n\n";

    xml << "    <net>\n";

    // Declare places with tokens for control places
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

        // Identify control input
        std::string control_input;
        if (net.transition_inputs.find(transition) != net.transition_inputs.end()) {
            for (const auto& input : net.transition_inputs.at(transition)) {
                auto it = net.place_types.find(input);
                if (it != net.place_types.end() && it->second == "control") {
                    control_input = input;
                    break;
                }
            }
        }

        // Identify global inputs
        std::vector<std::string> global_inputs;
        if (net.transition_inputs.find(transition) != net.transition_inputs.end()) {
            for (const auto& input : net.transition_inputs.at(transition)) {
                if (globals.count(input)) {
                    global_inputs.push_back(input);
                }
            }
        }

        // Identify data inputs
        std::vector<std::string> data_inputs;
        if (net.transition_inputs.find(transition) != net.transition_inputs.end()) {
            for (const auto& input : net.transition_inputs.at(transition)) {
                auto it = net.place_types.find(input);
                if (it != net.place_types.end() && it->second != "control" && globals.count(input) == 0) {
                    data_inputs.push_back(input);
                }
            }
        }

        // Get outputs
        std::vector<std::string> outputs;
        auto out_it = net.transition_outputs.find(transition);
        if (out_it != net.transition_outputs.end()) {
            outputs = out_it->second;
        }

        // Define inputs and outputs ports
        if (transition == "T_11") {
            // Special case for T_11: Add "data" as input and "data_11" as output
            xml << "                <in name=\"data\" type=\"feynman\"/>\n";
            for (const auto& output : outputs) {
                xml << "                <out name=\"" << output << "\" type=\"" 
                    << net.place_types.at(output) << "\"/>\n";
            }
        } else if (data_inputs.size() == 1 && transition != "T_end" && outputs.size() == 1) {
            std::string output_place = outputs[0];
            xml << "                <inout name=\"" << output_place << "\" type=\"" 
                << net.place_types.at(output_place) << "\"/>\n";
        } else {
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                std::string port_name = "data_in_" + std::to_string(i + 1);
                xml << "                <in name=\"" << port_name << "\" type=\"" 
                    << net.place_types.at(data_inputs[i]) << "\"/>\n";
            }
            for (const auto& output : outputs) {
                xml << "                <out name=\"" << output << "\" type=\"" 
                    << net.place_types.at(output) << "\"/>\n";
            }
        }

        // Global and control input ports
        for (const auto& global : global_inputs) {
            xml << "                <in name=\"" << global << "\" type=\"string\"/>\n";
        }
        if (!control_input.empty()) {
            xml << "                <in name=\"" << control_input << "\" type=\"control\"/>\n";
        }

        // Define function_args
        std::vector<std::string> function_args;
        if (!control_input.empty()) {
            function_args.push_back(control_input);
        }
        for (const auto& global : global_inputs) {
            function_args.push_back(global);
        }
        if (transition == "T_11") {
            function_args.push_back("data"); // Input for T_11
            for (const auto& output : outputs) {
                function_args.push_back(output); // e.g., data_11
            }
        } else if (data_inputs.size() == 1 && transition != "T_end" && outputs.size() == 1) {
            std::string output_place = outputs[0];
            function_args.push_back(output_place); // <inout>
        } else {
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                function_args.push_back("data_in_" + std::to_string(i + 1)); // <in>
            }
            for (const auto& output : outputs) {
                function_args.push_back(output); // <out>
            }
        }

        // Module definition
        xml << "                <module name=\"singular_template\" require_function_unloads_without_rest=\"false\" function=\"f_" << transition << "(";
        for (size_t i = 0; i < function_args.size(); ++i) {
            xml << function_args[i];
            if (i < function_args.size() - 1) {
                xml << ", ";
            }
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
            xml << "                        // Combine all data inputs into final_data\n";
            xml << "                        final_data.web = \"\"; // Initialize final_data.web\n";
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                xml << "                        final_data.web += data_in_" << (i + 1) << ".web;\n";
            }
            xml << "                        final_data.reducedIBPs = data_in_1.reducedIBPs; // Example combination\n";
            xml << "                        final_data.MI = data_in_1.MI;\n";
        } else {
            int j = 0, k = 0;
            if (transition.size() >= 4 && transition[0] == 'T' && transition[1] == '_' &&
                std::isdigit(transition[2]) && std::isdigit(transition[3])) {
                j = transition[2] - '0';
                k = transition[3] - '0';
            } else {
                std::cerr << "Warning: Unexpected transition format: " << transition << std::endl;
            }

            bool is_T_11 = (transition == "T_11");
            std::string input_data_ref = is_T_11 ? "data" : (data_inputs.size() == 1 ? data_inputs[0] : "data_in_1");
            std::string output_data_ref = outputs[0];

            xml << "                        int j = " << j << ", k = " << k << ";\n";
            xml << "                        std::cout << \"j = \" << j << \", k = \" << k << std::endl;\n";

            // Seed computation
            xml << "                        std::string reducedIBPs_seed = RESOLVE_INTERFACE_FUNCTION(singular_seed_gpi)("
                << input_data_ref << ".reducedIBPs, library_name, base_filename);\n";
            xml << "                        std::cout << \"reducedIBPs_seed: \" << reducedIBPs_seed << std::endl;\n";

            // Target integrals
            xml << "                        std::string targetInt = RESOLVE_INTERFACE_FUNCTION(singular_targetInt_gpi)(input, library_name, base_filename);\n";
            xml << "                        std::cout << \"Target Integrals: \" << targetInt << std::endl;\n";

            // Web replacement and target integrals
            xml << "                        std::string wjk = RESOLVE_INTERFACE_FUNCTION(singular_replace_two)("
                << input_data_ref << ".web, j, k, library_name, base_filename);\n";
            xml << "                        std::string wjk_targetInt = RESOLVE_INTERFACE_FUNCTION(singular_targetInts_gpi)(wjk, library_name, base_filename);\n";
            xml << "                        std::cout << \"web[\" << j << \",\" << k << \"].TARGET = \" << wjk_targetInt << std::endl;\n";
            xml << "                        int size_wjk_targetInt = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(wjk_targetInt, library_name, base_filename);\n";
            xml << "                        std::cout << \"size_wjk_targetInt = \" << size_wjk_targetInt << std::endl;\n";

            // Update seed and compute IBP if size >= 1
            xml << "                        reducedIBPs_seed = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(targetInt, library_name, base_filename);\n";
            xml << "                        std::cout << \"Updated reducedIBPs_seed: \" << reducedIBPs_seed << std::endl;\n";
            xml << "                        if (size_wjk_targetInt >= 1) {\n";
            xml << "                            std::string totalIBP = RESOLVE_INTERFACE_FUNCTION(singular_computeManyIBPjk_gpi)(labeledgraph, wjk_targetInt, library_name, base_filename);\n";
            xml << "                            std::string totalIBP_IBP = RESOLVE_INTERFACE_FUNCTION(singular_IBP_gpi)(totalIBP, library_name, base_filename);\n";
            xml << "                            int size_totalIBP_ibp = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(totalIBP_IBP, library_name, base_filename);\n";
            xml << "                            std::cout << \"Size of totalIBP_IBP: \" << size_totalIBP_ibp << std::endl;\n";

            // Update reducedIBPs if condition met
            xml << "                            int te = 0;\n";
            xml << "                            if (te == 0) {\n";
            xml << "                                std::string reducedibp_over = RESOLVE_INTERFACE_FUNCTION(singular_over_gpi)("
                << input_data_ref << ".reducedIBPs, library_name, base_filename);\n";
            xml << "                                std::string totalIBP_over = RESOLVE_INTERFACE_FUNCTION(singular_over_gpi)(totalIBP, library_name, base_filename);\n";
            xml << "                                reducedibp_over = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(totalIBP_over, library_name, base_filename);\n";
            xml << "                                te = 1;\n";
            xml << "                                std::cout << \"Updated reducedibp_over: \" << reducedibp_over << std::endl;\n";
            xml << "                            }\n";

            // Compute independent IBPs and masters
            xml << "                            std::string L = RESOLVE_INTERFACE_FUNCTION(singular_getRedIBPs_gpi)(totalIBP, 101, library_name, base_filename);\n";
            xml << "                            std::cout << \"L from getRedIBPs_gpi: \" << L << std::endl;\n";
            xml << "                            std::string indpndIBP = RESOLVE_INTERFACE_FUNCTION(singular_indpndIBP_gpi)(L, library_name, base_filename);\n";
            xml << "                            std::string masterAndTailIntgrals = RESOLVE_INTERFACE_FUNCTION(singular_masterAndTailIntgrals_gpi)(L, library_name, base_filename);\n";
            xml << "                            std::cout << \"Independent IBP: \" << indpndIBP << \", Master and Tail Integrals: \" << masterAndTailIntgrals << std::endl;\n";

            // Update reducedIBPs_IBP
            xml << "                            std::string reducedIBPs_IBP = RESOLVE_INTERFACE_FUNCTION(singular_IBP_gpi)("
                << input_data_ref << ".reducedIBPs, library_name, base_filename);\n";
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

            // Update web and masters
            xml << "                            std::string w = RESOLVE_INTERFACE_FUNCTION(singular_updateWeb_later)("
                << input_data_ref << ".web, masterAndTailIntgrals, j, k, library_name, base_filename);\n";
            xml << "                            std::string w1 = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(w, 1, library_name, base_filename);\n";
            xml << "                            " << output_data_ref << ".web = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(w1, library_name, base_filename);\n";
            xml << "                            std::string masterIntgralsInSector = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(w, 2, library_name, base_filename);\n";
            xml << "                            int masterIntgralsInSector_size = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)(masterIntgralsInSector, library_name, base_filename);\n";

            // Update MI
            xml << "                            int MI_size = RESOLVE_INTERFACE_FUNCTION(singular_size_gpi)("
                << input_data_ref << ".MI, library_name, base_filename);\n";
            xml << "                            if (MI_size == 0) {\n";
            xml << "                                " << output_data_ref << ".MI = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(masterIntgralsInSector, library_name, base_filename);\n";
            xml << "                                std::cout << \"Assigned master integrals to MI.\" << std::endl;\n";
            xml << "                            } else {\n";
            xml << "                                for (int i = 1; i <= masterIntgralsInSector_size; ++i) {\n";
            xml << "                                    std::string masterIntgralsInSector_i = RESOLVE_INTERFACE_FUNCTION(singular_Return_place)(masterIntgralsInSector, i, library_name, base_filename);\n";
            xml << "                                    int alreadyIntheList = RESOLVE_INTERFACE_FUNCTION(singular_alreadyIntheList_gpi)("
                << input_data_ref << ".MI, masterIntgralsInSector_i, library_name);\n";
            xml << "                                    if (!alreadyIntheList) {\n";
            xml << "                                        " << output_data_ref << ".MI = RESOLVE_INTERFACE_FUNCTION(singular_assign_gpi)(masterIntgralsInSector_i, library_name, base_filename);\n";
            xml << "                                        std::cout << \"Added master integral from sector index \" << i << std::endl;\n";
            xml << "                                    }\n";
            xml << "                                }\n";
            xml << "                            }\n";
            xml << "                        }\n";
            // Assign final values to output (example)
            xml << "                        " << output_data_ref << ".reducedIBPs = reducedIBPs_seed;\n";
        }

        xml << "                    ]]></code>\n";
        xml << "                </module>\n";
        xml << "            </defun>\n";

        // Connections
        if (transition == "T_11") {
            xml << "            <connect-in port=\"data\" place=\"data\"/>\n";
            for (const auto& output : outputs) {
                xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
            }
        } else if (data_inputs.size() == 1 && transition != "T_end" && outputs.size() == 1) {
            std::string output_place = outputs[0];
            std::string data_input_place = data_inputs[0];
            xml << "            <connect-read port=\"" << output_place << "\" place=\"" << data_input_place << "\"/>\n";
            xml << "            <connect-out port=\"" << output_place << "\" place=\"" << output_place << "\"/>\n";
        } else {
            for (size_t i = 0; i < data_inputs.size(); ++i) {
                std::string port_name = "data_in_" + std::to_string(i + 1);
                xml << "            <connect-read port=\"" << port_name << "\" place=\"" << data_inputs[i] << "\"/>\n";
            }
            for (const auto& output : outputs) {
                xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
            }
        }
        for (const auto& global : global_inputs) {
            xml << "            <connect-read port=\"" << global << "\" place=\"" << global << "\"/>\n";
        }
        if (!control_input.empty()) {
            xml << "            <connect-in port=\"" << control_input << "\" place=\"" << control_input << "\"/>\n";
        }

        xml << "        </transition>\n";
    }

    xml << "    </net>\n";
    xml << "</defun>\n";

    return xml.str();
}

void print_petri_net(const PetriNet& net) {
    std::cout << "\nGenerated PetriNet:\n";
    
    // Print places and their types
    std::cout << "Places:\n";
    for (const auto& place : net.places) {
        auto it = net.place_types.find(place);
        std::string type = (it != net.place_types.end()) ? it->second : "unknown";
        std::cout << "  " << place << " (" << type << ")\n";
    }
    
    // Print transitions
    std::cout << "\nTransitions:\n";
    for (const auto& t : net.transitions) {
        std::cout << "  " << t << "\n";
    }
    
    // Print transition inputs
    std::cout << "\nTransition Inputs:\n";
    for (const auto& [transition, inputs] : net.transition_inputs) {
        std::cout << "  " << transition << " <- { ";
        for (const auto& input : inputs) {
            std::cout << input << " ";
        }
        std::cout << "}\n";
    }
    
    // Print transition outputs
    std::cout << "\nTransition Outputs:\n";
    for (const auto& [transition, outputs] : net.transition_outputs) {
        std::cout << "  " << transition << " -> { ";
        for (const auto& output : outputs) {
            std::cout << output << " ";
        }
        std::cout << "}\n";
    }
}



// Main function
int main() {
    std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "41"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"22", "31"}, {"22", "32"},
        {"23", "32"}, {"31", "41"}, {"32", "41"}
    };

    DAG myDAG = generate_dag(vertices, edges);
    std::string root_node = find_root_node(myDAG);
    
    std::cout << "Root Node: " << root_node << "\n";

    PetriNet myPetriNet = generate_petri_net(myDAG);
    print_petri_net(myPetriNet);

    std::string xml = generateXPNetXMLWithT0Tend(myPetriNet);

    std::ofstream file("workflow/template.xpnet");
    file << xml;
    file.close();

    return 0;
}
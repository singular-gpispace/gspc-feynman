#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>
#include <chrono>

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

// Forward declarations for helper functions
std::string generateT0Code();
std::string generateT11Code();
std::string generateTEndCode(const std::vector<std::string>& inputs);
std::string generateRegularTransitionCode(const std::string& transition, const PetriNet& net, const DAG& dag);

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

// Generate Petri net
PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;

    // Add all transitions
    net.transitions = dag.vertices;
    net.transitions.push_back("T_0");
    net.transitions.push_back("T_end");

    // Define global places
    std::vector<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph", "web", "tail_00", "output"};
    for (const auto& g : globals) {
        net.places.push_back(g);
        net.place_types[g] = "string";
    }

    // Add control places and tail places for each transition
    for (const auto& transition : dag.vertices) {
        std::string ctrl = "control_" + transition.substr(2);
        std::string tail_out = "tail_" + transition.substr(2);

        net.places.push_back(ctrl);
        net.places.push_back(tail_out);
        net.place_types[ctrl] = "control";
        net.place_types[tail_out] = "string";
    }

    // Add control places for T_0 and T_end
    net.places.push_back("control_T_0");
    net.places.push_back("control_T_end");
    net.place_types["control_T_0"] = "control";
    net.place_types["control_T_end"] = "control";

    // Define T_0 inputs and outputs
    net.transition_inputs["T_0"] = {"control_T_0", "library_name", "base_filename", "input"};
    net.transition_outputs["T_0"] = {"labeledgraph", "web", "tail_00"};

    // Define inputs and outputs for regular transitions
    for (const auto& transition : dag.vertices) {
        // Inputs: control, globals, parent tail places
        net.transition_inputs[transition] = {"control_" + transition.substr(2)};
        // Add read-only globals
        net.transition_inputs[transition].push_back("labeledgraph");
        net.transition_inputs[transition].push_back("library_name");
        net.transition_inputs[transition].push_back("base_filename");
        net.transition_inputs[transition].push_back("web");
        net.transition_inputs[transition].push_back("tail_00");

        // Add input only for T_0
        if (transition == "T_0") {
            net.transition_inputs[transition].push_back("input");
        }

        // Add parent tails as inputs
        if (dag.parents.find(transition) != dag.parents.end()) {
            for (const auto& parent : dag.parents.at(transition)) {
                net.transition_inputs[transition].push_back("tail_" + parent.substr(2));
            }
        }

        // Add tail_11 as input to all transitions except T_0, T_11, and T_end
        // Only add if it's not already in the inputs (to prevent duplicates)
        if (transition != "T_0" && transition != "T_11" && transition != "T_end") {
            bool has_tail_11 = false;
            for (const auto& input : net.transition_inputs[transition]) {
                if (input == "tail_11") {
                    has_tail_11 = true;
                    break;
                }
            }
            if (!has_tail_11) {
                net.transition_inputs[transition].push_back("tail_11");
            }
        }

        // Outputs: tail output only
        net.transition_outputs[transition] = {"tail_" + transition.substr(2)};
    }

    // Define T_end inputs and outputs
    net.transition_inputs["T_end"] = {"control_T_end"};
    net.transition_outputs["T_end"] = {"output"};

    // Add all leaf node tails as inputs to T_end
    std::set<std::string> leaf_nodes(dag.vertices.begin(), dag.vertices.end());
    for (const auto& edge : dag.edges) {
        leaf_nodes.erase(edge.first);
    }
    for (const auto& leaf : leaf_nodes) {
        net.transition_inputs["T_end"].push_back("tail_" + leaf.substr(2));
    }

    return net;
}

// Generate XML
std::string generateXPNetXMLWithT0Tend(const PetriNet& net, const DAG& dag) {
    std::ostringstream xml;

    // XML header
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<defun name=\"template\">\n\n";

    // Top-level inputs and output
    xml << "    <in name=\"base_filename\" type=\"string\" place=\"base_filename\"/>\n";
    xml << "    <in name=\"library_name\" type=\"string\" place=\"library_name\"/>\n";
    xml << "    <in name=\"input\" type=\"string\" place=\"input\"/>\n";
    xml << "    <out name=\"output\" type=\"string\" place=\"output\"/>\n\n";
    xml << "    <net>\n";

    // Declare places
    for (const auto& place : net.places) {
        xml << "        <place name=\"" << place << "\" type=\"" << net.place_types.at(place) << "\">\n";
        if (net.place_types.at(place) == "control") {
            xml << "            <token><value>[]</value></token>\n";
        }
        xml << "        </place>\n";
    }

    // Generate transitions
    for (const auto& transition : net.transitions) {
        xml << "        <transition name=\"" << transition << "\">\n";
        xml << "            <defun>\n";
        xml << "                <require key=\"worker\" mandatory=\"true\"/>\n\n";

        // Port declarations
        if (transition == "T_0") {
            xml << "                <in name=\"control_T_0\" type=\"control\"/>\n";
            xml << "                <in name=\"library_name\" type=\"string\"/>\n";
            xml << "                <in name=\"base_filename\" type=\"string\"/>\n";
            xml << "                <in name=\"input\" type=\"string\"/>\n";
            xml << "                <out name=\"labeledgraph\" type=\"string\"/>\n";
            xml << "                <out name=\"web\" type=\"string\"/>\n";
            xml << "                <out name=\"tail_00\" type=\"string\"/>\n";
        } else if (transition == "T_end") {
            xml << "                <in name=\"control_T_end\" type=\"control\"/>\n";
            for (const auto& input : net.transition_inputs.at(transition)) {
                if (input != "control_T_end") {
                    xml << "                <in name=\"" << input << "\" type=\"string\"/>\n";
                }
            }
            xml << "                <out name=\"output\" type=\"string\"/>\n";
        } else {
            // Regular transition ports
            for (const auto& input : net.transition_inputs.at(transition)) {
                xml << "                <in name=\"" << input << "\" type=\"" << net.place_types.at(input) << "\"/>\n";
            }
            for (const auto& output : net.transition_outputs.at(transition)) {
                xml << "                <out name=\"" << output << "\" type=\"string\"/>\n";
            }
        }

        // Module definition
        xml << "                <module name=\"singular_template\" require_function_unloads_without_rest=\"false\" ";
        xml << "function=\"f_" << transition << "(";
        
        // Function arguments
        bool first = true;
        for (const auto& input : net.transition_inputs.at(transition)) {
            if (!first) xml << ", ";
            xml << input;
            first = false;
        }
        for (const auto& output : net.transition_outputs.at(transition)) {
            if (!first) xml << ", ";
            xml << output;
            first = false;
        }
        xml << ")\">\n";

        // Include statements
        xml << "                    <cinclude href=\"interface/template_interface.hpp\"/>\n";
        xml << "                    <cinclude href=\"util-generic/dynamic_linking.hpp\"/>\n";
        xml << "                    <code><![CDATA[\n";

        // Transition-specific code
        if (transition == "T_0") {
            xml << generateT0Code();
        } else if (transition == "T_end") {
            xml << generateTEndCode(net.transition_inputs.at(transition));
        } else {
            xml << generateRegularTransitionCode(transition, net, dag);
        }

        xml << "                    ]]></code>\n";
        xml << "                </module>\n";
        xml << "            </defun>\n";

        // Connections
        for (const auto& input : net.transition_inputs.at(transition)) {
            if (input.substr(0, 7) == "control") {
                xml << "            <connect-in port=\"" << input << "\" place=\"" << input << "\"/>\n";
            } else {
                xml << "            <connect-read port=\"" << input << "\" place=\"" << input << "\"/>\n";
            }
        }
        for (const auto& output : net.transition_outputs.at(transition)) {
            xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
        }

        xml << "        </transition>\n\n";
    }

    xml << "    </net>\n";
    xml << "</defun>\n";
    return xml.str();
}

// Helper function for T_0
std::string generateT0Code() {
    std::ostringstream code;
    code << "                        std::cout << \"Starting T_0...\\n\";\n";
    code << "                        labeledgraph = RESOLVE_INTERFACE_FUNCTION(singular_getBaikovMatrix_gpi)(input, library_name, base_filename);\n";
    code << "                        web = RESOLVE_INTERFACE_FUNCTION(singular_web_gpi)(input, library_name, base_filename);\n";
    code << "                        tail_00 = RESOLVE_INTERFACE_FUNCTION(singular_targetInt_gpi)(input, library_name, base_filename);\n";
    code << "                        std::cout << \"T_0 completed\\n\";\n";
    return code.str();
}

// Helper function for T_11
std::string generateT11Code() {
    std::ostringstream code;
    code << "                        std::cout << \"Starting T_11...\\n\";\n";
    code << "                        std::string input_11 = RESOLVE_INTERFACE_FUNCTION(singular_makeUserInput_gpi)(tail_00, library_name, base_filename);\n";
    code << "                        std::string labels_11 = RESOLVE_INTERFACE_FUNCTION(singular_getlabels_gpi)(web, 1, 1, library_name, base_filename);\n";
    code << "                        std::string one_sector_11 = RESOLVE_INTERFACE_FUNCTION(singular_computeSector_flint_cpp)(labeledgraph, input_11, labels_11, 7853, library_name, base_filename);\n";
    code << "                        std::string size_of_one_sector = RESOLVE_INTERFACE_FUNCTION(singular_size_computeSector_gpi)(one_sector_11, library_name, base_filename);\n";
    code << "                        std::cout << \"size of one_sector_11: IBP_11, MI_11, Tail_11=\" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(size_of_one_sector, library_name) << std::endl;\n";
    code << "                        tail_11 = RESOLVE_INTERFACE_FUNCTION(singular_gettail_gpi)(one_sector_11, library_name, base_filename);\n";
    code << "                        std::cout << \"T_11 completed\\n\";\n";
    return code.str();
}

// Helper function for T_end
std::string generateTEndCode(const std::vector<std::string>& inputs) {
    std::ostringstream code;
    code << "                        std::cout << \"Starting T_end...\\n\";\n";
    code << "                        output = \"\";\n";
    bool first = true;
    for (const auto& input : inputs) {
        if (input.substr(0, 5) == "tail_") {
            if (!first) code << "                        output += \";\";\n";
            code << "                        output += " << input << ";\n";
            first = false;
        }
    }
    code << "                        std::cout << \"T_end completed\\n\";\n";
    return code.str();
}

// Helper function for regular transitions (T_xx)
std::string generateRegularTransitionCode(const std::string& transition, const PetriNet& net, const DAG& dag) {
    std::ostringstream code;
    code << "                        std::cout << \"Starting " << transition << "...\\n\";\n\n";

    // Extract j, k from transition name (e.g., T_417 -> j=4, k=17)
    std::string jk = transition.substr(2);
    if (jk.empty()) return code.str();
    
    // First digit is j, rest is k
    std::string j = jk.substr(0, 1);
    std::string k = jk.substr(1);

    // Define jk_pairs based on inputs (exclude current j,k)
    std::vector<std::pair<int, int>> jk_pairs = {{0, 0}};
    const auto& inputs = net.transition_inputs.at(transition);
    for (const auto& input : inputs) {
        if (input.substr(0, 5) == "tail_" && input != "tail_00") {
            std::string tail_jk = input.substr(5);
            if (!tail_jk.empty()) {
                int tail_j = std::stoi(tail_jk.substr(0, 1));
                int tail_k = std::stoi(tail_jk.substr(1));
                jk_pairs.emplace_back(tail_j, tail_k);
            }
        }
    }

    // Generate jk_pairs vector
    code << "                        std::vector<std::pair<int, int>> jk_pairs = {";
    bool first = true;
    for (const auto& [pj, pk] : jk_pairs) {
        if (!first) code << ", ";
        code << "{" << pj << ", " << pk << "}";
        first = false;
    }
    code << "};\n";

    // Generate tail_jk map
    code << "                        std::map<std::pair<int, int>, std::string> tail_jk;\n";
    for (const auto& input : inputs) {
        if (input.substr(0, 5) == "tail_") {
            std::string tail_jk = input.substr(5);
            if (!tail_jk.empty()) {
                int tail_j = std::stoi(tail_jk.substr(0, 1));
                int tail_k = std::stoi(tail_jk.substr(1));
                code << "                        tail_jk[{" << tail_j << ", " << tail_k << "}] = " << input << ";\n";
            }
        }
    }

    // Generate input construction
    code << "\n                        std::string input_" << jk << " = RESOLVE_INTERFACE_FUNCTION(singular_makeUserInput_gpi)(tail_00, library_name, base_filename);\n";
    code << "                        for (size_t i = 1; i < jk_pairs.size(); i++) {\n";
    code << "                            auto [j, k] = jk_pairs[i];\n";
    code << "                            std::string tail = tail_jk[{j, k}];\n";
    code << "                            input_" << jk << " = RESOLVE_INTERFACE_FUNCTION(singular_append_list_gpi)(input_" << jk << ", tail, library_name, base_filename);\n";
    code << "                        }\n";

    // Generate labels and sector computation
    code << "\n                        std::string labels_" << jk << " = RESOLVE_INTERFACE_FUNCTION(singular_getlabels_gpi)(web, " << j << ", " << k << ", library_name, base_filename);\n";
    code << "                        std::cout << \"labels_" << jk << ":\" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(labels_" << jk << ", library_name) << std::endl;\n";
    code << "/******************************************************************************************************************************************* */\n";
    code << "                        std::cout << \"Starting one sector computation in " << transition << "\\n\";\n";
    code << "                        auto computation_time_" << jk << " = std::chrono::high_resolution_clock::now();\n";
    code << "                        std::string one_sector_" << jk << " = RESOLVE_INTERFACE_FUNCTION(singular_computeSector_flint_cpp)(labeledgraph, input_" << jk << ", labels_" << jk << ", 7853, library_name, base_filename);\n";
    code << "                        RESOLVE_INTERFACE_FUNCTION(singular_remove_file)(input_" << jk << ");\n";
    code << "                        auto computation_end_time_" << jk << " = std::chrono::high_resolution_clock::now();\n";
    code << "                        auto computation_duration_" << jk << " = std::chrono::duration_cast<std::chrono::milliseconds>(computation_end_time_" << jk << " - computation_time_" << jk << ");\n";
    code << "                        std::cout << \"One sector computation completed in " << transition << " in \" << computation_duration_" << jk << ".count() << \" milliseconds\\n\";\n";
    code << "/******************************************************************************************************************************************* */\n";
    code << "                        std::string size_of_one_sector = RESOLVE_INTERFACE_FUNCTION(singular_size_OneSectorComputation_gpi)(one_sector_" << jk << ", library_name, base_filename);\n";
    code << "                        std::cout << \"size of one_sector_" << jk << ": IBP_" << jk << ", MI_" << jk << ", Tail_" << jk << "=\" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(size_of_one_sector, library_name) << std::endl;\n\n";

    // Generate output tail
    code << "\n                        tail_" << jk << " = RESOLVE_INTERFACE_FUNCTION(singular_gettail_gpi)(one_sector_" << jk << ", library_name, base_filename);\n";
    code << "                        RESOLVE_INTERFACE_FUNCTION(singular_remove_file)(one_sector_" << jk << ");\n";
    code << "                        std::cout << \"" << transition << " completed\\n\";\n";

    return code.str();
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
    std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "33"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"21", "32"}, {"22", "32"}, {"22", "33"}, {"23", "31"}, {"23", "33"}
    }; 
/* 
  std::vector<std::string> vertices = {"11", "21", "22", "23", "24", "31", "32", "33", "34", "35", "36", "41", "42", "43", "44"};
 std::vector<std::pair<std::string, std::string>> edges =
{{"11", "21"}, {"11", "22"}, {"11", "23"}, {"11", "24"}, {"21", "31"}, {"21", "32"}, {"21", "34"}, {"22", "31"}, {"22", "33"}, {"22", "35"}, {"23", "32"}, {"23", "33"}, {"23", "36"}, {"24", "34"}, {"24", "35"}, {"24", "36"}, {"31", "41"}, {"31", "42"}, {"32", "41"}, {"32", "43"}, {"33", "41"}, {"33", "44"}, {"34", "42"}, {"34", "43"}, {"35", "42"}, {"35", "44"}, {"36", "43"}, {"36", "44"}};
  */
   DAG myDAG = generate_dag(vertices, edges);
    std::string root_node = find_root_node(myDAG);
    std::cout << "Root Node: " << root_node << "\n";

    PetriNet myPetriNet = generate_petri_net(myDAG);
    print_petri_net(myPetriNet);

    std::string xml = generateXPNetXMLWithT0Tend(myPetriNet, myDAG);
    std::ofstream file("generated_flint_cpp.xpnet");
    file << xml;
    file.close();

    return 0;
}
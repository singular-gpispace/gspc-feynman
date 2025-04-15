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

// Forward declarations for helper functions
std::string generateT0Code();
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

PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;

    // Add all transitions
    net.transitions = dag.vertices;
    net.transitions.push_back("T_0");
    net.transitions.push_back("T_end");

    // Define global places
    std::vector<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph", "web", "tail_00"};
    for (const auto& g : globals) {
        net.places.push_back(g);
        net.place_types[g] = "string";
    }

    // Add per-transition tail places and control places
    for (const auto& transition : dag.vertices) {
        std::string ctrl = "control_" + transition.substr(2);
        std::string tail_out = "tail_" + transition.substr(2);

        net.places.push_back(ctrl);
        net.places.push_back(tail_out);
        net.place_types[ctrl] = "control";
        net.place_types[tail_out] = "string";

        // Inputs: control, globals, parent tail places
        net.transition_inputs[transition] = {ctrl};
        // Add read-only globals
        net.transition_inputs[transition].push_back("labeledgraph");
        net.transition_inputs[transition].push_back("library_name");
        net.transition_inputs[transition].push_back("base_filename");
        net.transition_inputs[transition].push_back("input");
        net.transition_inputs[transition].push_back("web");
        net.transition_inputs[transition].push_back("tail_00");

        // Add parent tails as inputs
        if (dag.parents.find(transition) != dag.parents.end()) {
            for (const auto& parent : dag.parents.at(transition)) {
                net.transition_inputs[transition].push_back("tail_" + parent.substr(2));
            }
        }

        // Outputs: tail output only
        net.transition_outputs[transition] = {tail_out};
    }

    // Set up T_0 (initial transition)
    net.places.push_back("control_T_0");
    net.place_types["control_T_0"] = "control";
    net.transition_inputs["T_0"] = {"control_T_0", "library_name", "base_filename", "input"};
    net.transition_outputs["T_0"] = {"labeledgraph", "web", "tail_00"};

    // Set up T_end (final transition)
    net.places.push_back("control_T_end");
    net.places.push_back("output");  // Changed from final_tail to output
    net.place_types["control_T_end"] = "control";
    net.place_types["output"] = "string";
    net.transition_inputs["T_end"] = {"control_T_end"};

    // Find leaf transitions (those not input to any other transition)
    std::set<std::string> leaf_transitions(dag.vertices.begin(), dag.vertices.end());
    for (const auto& edge : dag.edges) {
        leaf_transitions.erase(edge.first);
    }
    
    // Add leaf transition tails as inputs to T_end
    for (const auto& leaf : leaf_transitions) {
        net.transition_inputs["T_end"].push_back("tail_" + leaf.substr(2));
    }
    net.transition_outputs["T_end"] = {"output"};

    return net;
}

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
        auto it = net.place_types.find(place);
        if (it != net.place_types.end()) {
            xml << "        <place name=\"" << place << "\" type=\"" << it->second << "\">\n";
            if (it->second == "control") {
                xml << "            <token><value>[]</value></token>\n";
            }
            xml << "        </place>\n";
        }
    }

    // Generate transitions
    for (const auto& transition : net.transitions) {
        xml << "        <transition name=\"" << transition << "\">\n";
        xml << "            <defun>\n";
        xml << "                <require key=\"worker\" mandatory=\"true\"/>\n\n";

        // Port declarations
        if (transition == "T_0") {
            // T_0 specific ports
            xml << "                <in name=\"base_filename\" type=\"string\"/>\n";
            xml << "                <in name=\"library_name\" type=\"string\"/>\n";
            xml << "                <in name=\"input\" type=\"string\"/>\n";
            xml << "                <in name=\"control_T_0\" type=\"control\"/>\n";
            xml << "                <out name=\"labeledgraph\" type=\"string\"/>\n";
            xml << "                <out name=\"web\" type=\"string\"/>\n";
            xml << "                <out name=\"tail_00\" type=\"string\"/>\n";
        } else if (transition == "T_end") {
            // T_end specific ports
            xml << "                <in name=\"control_T_end\" type=\"control\"/>\n";
            const auto& inputs = net.transition_inputs.at(transition);
            for (const auto& input : inputs) {
                if (input.substr(0, 5) == "tail_") {
                    xml << "                <in name=\"" << input << "\" type=\"string\"/>\n";
                }
            }
            xml << "                <out name=\"output\" type=\"string\"/>\n";
        } else {
            // Regular transition ports
            const auto& inputs = net.transition_inputs.at(transition);
            for (const auto& input : inputs) {
                xml << "                <in name=\"" << input << "\" type=\"" << net.place_types.at(input) << "\"/>\n";
            }
            const auto& outputs = net.transition_outputs.at(transition);
            for (const auto& output : outputs) {
                xml << "                <out name=\"" << output << "\" type=\"" << net.place_types.at(output) << "\"/>\n";
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
        const auto& inputs = net.transition_inputs.at(transition);
        for (const auto& input : inputs) {
            if (input.substr(0, 7) == "control") {
                xml << "            <connect-in port=\"" << input << "\" place=\"" << input << "\"/>\n";
            } else {
                xml << "            <connect-read port=\"" << input << "\" place=\"" << input << "\"/>\n";
            }
        }
        const auto& outputs = net.transition_outputs.at(transition);
        for (const auto& output : outputs) {
            xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
        }

        xml << "        </transition>\n\n";
    }

    xml << "    </net>\n";
    xml << "</defun>\n";
    return xml.str();
}

// Helper functions implementation
std::string generateT0Code() {
    std::ostringstream code;
    code << "                        std::cout << \"Starting T_0...\" << std::endl;\n";
    code << "                        labeledgraph = RESOLVE_INTERFACE_FUNCTION(singular_getBaikovMatrix_gpi)(input, library_name, base_filename);\n";
    code << "                        web = RESOLVE_INTERFACE_FUNCTION(singular_web_gpi)(input, library_name, base_filename);\n";
    code << "                        tail_00 = RESOLVE_INTERFACE_FUNCTION(singular_targetInt_gpi)(input, library_name, base_filename);\n";
    code << "                        std::cout << \"T_0 completed\" << std::endl;\n";
    return code.str();
}

std::string generateTEndCode(const std::vector<std::string>& inputs) {
    std::ostringstream code;
    code << "                        std::cout << \"Starting T_end...\" << std::endl;\n";
    code << "                        output = \"\";\n";
    bool first = true;
    for (const auto& input : inputs) {
        if (input.substr(0, 5) == "tail_") {
            if (!first) code << "                        output += \";\";\n";
            code << "                        output += " << input << ";\n";
            first = false;
        }
    }
    code << "                        std::cout << \"T_end completed\" << std::endl;\n";
    return code.str();
}

std::string generateRegularTransitionCode(const std::string& transition, const PetriNet& net, const DAG& dag) {
    std::ostringstream code;
    code << "                        std::cout << \"Starting " << transition << "...\" << std::endl;\n\n";
    
    // Extract j, k from transition name
    std::string j = transition.substr(2, 1);
    std::string k = transition.substr(3, 1);

    // Generate input list construction code
    code << "                        std::string input_" << j << k << " = RESOLVE_INTERFACE_FUNCTION(singular_makeUserInput_gpi)(tail_00, library_name, base_filename);\n";
    
    // Add parent tails
    if (dag.parents.find(transition) != dag.parents.end()) {
        for (const auto& parent : dag.parents.at(transition)) {
            std::string parent_j = parent.substr(2, 1);
            std::string parent_k = parent.substr(3, 1);
            code << "                        input_" << j << k << " = RESOLVE_INTERFACE_FUNCTION(singular_append_list_gpi)(input_" << j << k;
            code << ", tail_" << parent_j << parent_k << ", library_name, base_filename);\n";
        }
    }

    // Generate sector computation code
    code << "\n                        std::string labels_" << j << k << " = RESOLVE_INTERFACE_FUNCTION(singular_getlabels_gpi)(web, " << j << ", " << k << ", library_name, base_filename);\n";
    code << "                        std::string one_sector_" << j << k << " = RESOLVE_INTERFACE_FUNCTION(singular_OneSectorComputation_gpi)(labeledgraph, input_" << j << k << ", labels_" << j << k << ", library_name, base_filename);\n";
    code << "                        std::string size_of_one_sector = RESOLVE_INTERFACE_FUNCTION(singular_size_OneSectorComputation_gpi)(one_sector_" << j << k << ", library_name, base_filename);\n";
    code << "                        std::cout << \"size of one_sector_" << j << k << ": IBP_" << j << k << ", MI_" << j << k << ", Tail_" << j << k << "=\" << RESOLVE_INTERFACE_FUNCTION(printGpiTokenContent)(size_of_one_sector, library_name) << std::endl;\n\n";
    code << "                        tail_" << j << k << " = RESOLVE_INTERFACE_FUNCTION(singular_gettail_gpi)(one_sector_" << j << k << ", library_name, base_filename);\n";
    code << "                        std::cout << \"" << transition << " completed\" << std::endl;\n";
    
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

    DAG myDAG = generate_dag(vertices, edges);
    std::string root_node = find_root_node(myDAG);
    std::cout << "Root Node: " << root_node << "\n";

    PetriNet myPetriNet = generate_petri_net(myDAG);
    print_petri_net(myPetriNet);

    std::string xml = generateXPNetXMLWithT0Tend(myPetriNet, myDAG); // Pass myDAG
    std::ofstream file("generated.xpnet");
    file << xml;
    file.close();

    return 0;
}
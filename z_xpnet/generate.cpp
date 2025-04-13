#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>

// Mock Label structure
struct Label {
    std::vector<int> lab; // e.g., {1,2,3}
};

// Mock Web structure (T[i][j].lab)
using Web = std::vector<std::vector<Label>>;

// Define DAG structure with labels
struct DAG {
    std::vector<std::string> vertices; // e.g., T_11, T_21
    std::map<std::string, std::vector<int>> labels; // e.g., T_11 -> {1,2,3}
    std::vector<std::pair<std::string, std::string>> edges;
    std::map<std::string, std::vector<std::string>> parents;
};

// Define PetriNet structure
struct PetriNet {
    std::vector<std::string> places;
    std::map<std::string, std::string> place_types;
    std::vector<std::string> transitions;
    std::map<std::string, std::vector<int>> transition_labels; // Labels for transitions
    std::map<std::string, std::vector<std::string>> transition_inputs;
    std::map<std::string, std::vector<std::string>> transition_outputs;
};

// Helper: Check if small is a subset of big
bool subset(const std::vector<int>& small, const std::vector<int>& big) {
    for (int x : small) {
        if (std::find(big.begin(), big.end(), x) == big.end()) {
            return false;
        }
    }
    return true;
}

// C++ version of getGraph
struct GraphData {
    std::vector<std::pair<std::vector<int>, std::vector<int>>> verticesWithLabels; // [ [i,j], lab ]
    std::vector<std::pair<std::vector<int>, std::vector<int>>> edges; // [ [i,j], [i',j'] ]
};

GraphData getGraph(const Web& T) {
    GraphData result;
    int v_idx = 0;
    int e_idx = 0;

    std::cout << "Size of web T: " << T.size() << "\n";

    // Generate vertices with labels
    for (size_t i = 0; i < T.size(); ++i) {
        std::cout << "Size of layer " << (i + 1) << ": " << T[i].size() << "\n";
        for (size_t j = 0; j < T[i].size(); ++j) {
            std::vector<int> vertex = {static_cast<int>(i + 1), static_cast<int>(j + 1)};
            result.verticesWithLabels.emplace_back(vertex, T[i][j].lab);
            v_idx++;
        }
    }

    // Print vertices with labels
    std::cout << "Vertices with Labels:\n";
    for (const auto& [vertex, label] : result.verticesWithLabels) {
        std::cout << "(" << vertex[0] << ", " << vertex[1] << ")    Label: {";
        for (size_t k = 0; k < label.size(); ++k) {
            std::cout << label[k];
            if (k < label.size() - 1) std::cout << ",";
        }
        std::cout << "}\n";
    }

    // Generate edges
    for (size_t i = 0; i < T.size() - 1; ++i) {
        for (size_t j = 0; j < T[i].size(); ++j) {
            for (size_t k = 0; k < T[i + 1].size(); ++k) {
                if (subset(T[i + 1][k].lab, T[i][j].lab)) {
                    std::vector<int> from = {static_cast<int>(i + 1), static_cast<int>(j + 1)};
                    std::vector<int> to = {static_cast<int>(i + 2), static_cast<int>(k + 1)};
                    result.edges.emplace_back(from, to);
                    e_idx++;
                }
            }
        }
    }

    // Print edges
    std::cout << "Edges:\n";
    for (const auto& [from, to] : result.edges) {
        std::cout << "(" << from[0] << ", " << from[1] << ") -> ("
                  << to[0] << ", " << to[1] << ")\n";
    }

    return result;
}

// Mock function to generate web from targetInt
Web generateWeb(const std::vector<std::vector<int>>& targetInt) {
    Web T(3);
    T[0].push_back(Label{{1, 2, 3}});
    T[1].push_back(Label{{1, 2}});
    T[1].push_back(Label{{1, 3}});
    T[1].push_back(Label{{2, 3}});
    T[2].push_back(Label{{1}});
    T[2].push_back(Label{{2}});
    T[2].push_back(Label{{3}});
    return T;
}

// Generate DAG from getGraph output
DAG generate_dag(const GraphData& graphData) {
    DAG dag;
    for (const auto& [vertex, label] : graphData.verticesWithLabels) {
        std::string v = "T_" + std::to_string(vertex[0]) + std::to_string(vertex[1]);
        dag.vertices.push_back(v);
        dag.labels[v] = label;
    }
    for (const auto& [from, to] : graphData.edges) {
        std::string from_v = "T_" + std::to_string(from[0]) + std::to_string(from[1]);
        std::string to_v = "T_" + std::to_string(to[0]) + std::to_string(to[1]);
        dag.edges.emplace_back(from_v, to_v);
        dag.parents[to_v].push_back(from_v);
    }
    return dag;
}

// Find root node
std::string find_root_node(const DAG& dag) {
    std::set<std::string> root_nodes(dag.vertices.begin(), dag.vertices.end());
    for (const auto& edge : dag.edges) {
        root_nodes.erase(edge.second);
    }
    return root_nodes.empty() ? "" : *root_nodes.begin();
}

// Generate Petri net with labels
PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;

    // Add transitions from DAG vertices, plus T_0 and T_end
    net.transitions = dag.vertices;
    net.transitions.push_back("T_0");
    net.transitions.push_back("T_end");

    // Copy labels to transitions
    net.transition_labels = dag.labels;

    // Define global places
    std::vector<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};
    for (const auto& g : globals) {
        net.places.push_back(g);
        net.place_types[g] = "string";
    }

    // Add global field places (for initial tailInts)
    net.places.push_back("tailInts");
    net.place_types["tailInts"] = "string";

    // Add places and connections for DAG transitions
    for (const auto& transition : dag.vertices) {
        std::string suffix = transition.substr(2); // e.g., "11" from T_11
        std::string ctrl = "control_" + suffix;
        std::string mi_out = "MI_" + suffix;
        std::string tail_out = "tailInts_" + suffix;
        std::string ribps_out = "reducedIBPs_" + suffix;

        net.places.push_back(ctrl);
        net.places.push_back(mi_out);
        net.places.push_back(tail_out);
        net.places.push_back(ribps_out);
        net.place_types[ctrl] = "control";
        net.place_types[mi_out] = "string";
        net.place_types[tail_out] = "string";
        net.place_types[ribps_out] = "string";

        // Inputs: control, globals, global tailInts, parent outputs
        net.transition_inputs[transition] = {ctrl, "labeledgraph", "tailInts"};
        net.transition_inputs[transition].insert(
            net.transition_inputs[transition].end(),
            globals.begin(), globals.end()
        );
        if (dag.parents.find(transition) != dag.parents.end()) {
            for (const auto& parent : dag.parents.at(transition)) {
                std::string parent_suffix = parent.substr(2);
                net.transition_inputs[transition].push_back("MI_" + parent_suffix);
                net.transition_inputs[transition].push_back("tailInts_" + parent_suffix);
                net.transition_inputs[transition].push_back("reducedIBPs_" + parent_suffix);
            }
        }

        // Outputs: MI, tailInts, reducedIBPs
        net.transition_outputs[transition] = {mi_out, tail_out, ribps_out};
    }

    // Set up T_0
    net.places.push_back("control_T_0");
    net.place_types["control_T_0"] = "control";
    net.transition_inputs["T_0"] = {"control_T_0", "library_name", "base_filename", "input"};
    net.transition_outputs["T_0"] = {"labeledgraph", "tailInts"};

    // Set up T_end
    net.places.push_back("control_T_end");
    net.places.push_back("final_MI");
    net.places.push_back("final_tailInts");
    net.places.push_back("final_reducedIBPs");
    net.place_types["control_T_end"] = "control";
    net.place_types["final_MI"] = "string";
    net.place_types["final_tailInts"] = "string";
    net.place_types["final_reducedIBPs"] = "string";
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
            std::string suffix = transition.substr(2);
            net.transition_inputs["T_end"].push_back("MI_" + suffix);
            net.transition_inputs["T_end"].push_back("tailInts_" + suffix);
            net.transition_inputs["T_end"].push_back("reducedIBPs_" + suffix);
        }
    }
    net.transition_outputs["T_end"] = {"final_MI", "final_tailInts", "final_reducedIBPs"};

    return net;
}

// Modified generateXPNetXMLWithT0Tend
std::string generateXPNetXMLWithT0Tend(const PetriNet& net) {
    std::ostringstream xml;
    std::set<std::string> globals = {"base_filename", "library_name", "input", "labeledgraph"};

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<defun name=\"template\">\n\n";

    // Top-level inputs and outputs
    xml << "    <in name=\"base_filename\" type=\"string\" place=\"base_filename\"/>\n";
    xml << "    <in name=\"library_name\" type=\"string\" place=\"library_name\"/>\n";
    xml << "    <in name=\"input\" type=\"string\" place=\"input\"/>\n";
    xml << "    <out name=\"output_MI\" type=\"string\" place=\"final_MI\"/>\n";
    xml << "    <out name=\"output_tailInts\" type=\"string\" place=\"final_tailInts\"/>\n";
    xml << "    <out name=\"output_reducedIBPs\" type=\"string\" place=\"final_reducedIBPs\"/>\n\n";
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

        // Get label (if not T_0 or T_end)
        std::vector<int> label;
        if (net.transition_labels.count(transition)) {
            label = net.transition_labels.at(transition);
        }

        // Identify ports
        std::string control_input;
        std::vector<std::string> global_inputs;
        std::vector<std::string> field_inputs, field_outputs;
        std::set<std::string> inputs_set;

        if (net.transition_inputs.count(transition)) {
            for (const auto& input : net.transition_inputs.at(transition)) {
                auto it = net.place_types.find(input);
                if (it != net.place_types.end()) {
                    if (it->second == "control") {
                        control_input = input;
                    } else if (globals.count(input)) {
                        global_inputs.push_back(input);
                    } else if (input == "tailInts" && transition != "T_0") {
                        global_inputs.push_back(input);
                    } else {
                        inputs_set.insert(input);
                        field_inputs.push_back(input);
                    }
                }
            }
        }
        if (net.transition_outputs.count(transition)) {
            for (const auto& output : net.transition_outputs.at(transition)) {
                field_outputs.push_back(output);
            }
        }

        // Define ports
        for (const auto& input : field_inputs) {
            xml << "                <in name=\"" << input << "\" type=\"" << net.place_types.at(input) << "\"/>\n";
        }
        for (const auto& output : field_outputs) {
            xml << "                <out name=\"" << output << "\" type=\"" << net.place_types.at(output) << "\"/>\n";
        }
        for (const auto& global : global_inputs) {
            xml << "                <in name=\"" << global << "\" type=\"string\"/>\n";
        }
        if (!control_input.empty()) {
            xml << "                <in name=\"" << control_input << "\" type=\"control\"/>\n";
        }

        // Function arguments
        std::vector<std::string> function_args;
        if (!control_input.empty()) function_args.push_back(control_input);
        function_args.insert(function_args.end(), global_inputs.begin(), global_inputs.end());
        function_args.insert(function_args.end(), field_inputs.begin(), field_inputs.end());
        function_args.insert(function_args.end(), field_outputs.begin(), field_outputs.end());

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
            xml << "                        labeledgraph = RESOLVE_INTERFACE_FUNCTION(singular_getLabeledGraph_gpi)(input, library_name, base_filename);\n";
            xml << "                        tailInts = RESOLVE_INTERFACE_FUNCTION(singular_initTailInts_gpi)(input, library_name, base_filename);\n";
        } else if (transition == "T_end") {
            xml << "                        final_MI = \"\";\n";
            xml << "                        final_tailInts = \"\";\n";
            xml << "                        final_reducedIBPs = \"\";\n";
            for (size_t i = 0; i < field_inputs.size(); i += 3) { // Step by 3: MI, tailInts, reducedIBPs
                xml << "                        final_MI += " << field_inputs[i] << " + \";\";\n";
                xml << "                        final_tailInts += " << field_inputs[i + 1] << " + \";\";\n";
                xml << "                        final_reducedIBPs += " << field_inputs[i + 2] << " + \";\";\n";
            }
        } else {
            // Define sector label
            xml << "                        std::vector<int> sector_label = {";
            for (size_t i = 0; i < label.size(); ++i) {
                xml << label[i];
                if (i < label.size() - 1) xml << ", ";
            }
            xml << "};\n";

            // Aggregate tailInts from parents and global tailInts
            xml << "                        std::list<std::string> tailInts_list;\n";
            for (size_t i = 0; i < field_inputs.size(); i += 3) {
                xml << "                        tailInts_list.push_back(" << field_inputs[i + 1] << ");\n";
            }
            xml << "                        tailInts_list.push_back(tailInts);\n";

            // Call OneSectorComputation
            xml << "                        auto result = RESOLVE_INTERFACE_FUNCTION(singular_OneSectorComputation_gpi)(\n";
            xml << "                            labeledgraph,\n";
            xml << "                            tailInts_list,\n";
            xml << "                            sector_label,\n";
            xml << "                            library_name,\n";
            xml << "                            base_filename\n";
            xml << "                        );\n";

            // Assign outputs
            xml << "                        " << field_outputs[0] << " = std::get<1>(result); // MI\n";
            xml << "                        " << field_outputs[1] << " = std::get<2>(result); // tailInts\n";
            xml << "                        " << field_outputs[2] << " = std::get<0>(result); // reducedIBPs\n";
        }

        xml << "                    ]]></code>\n";
        xml << "                </module>\n";
        xml << "            </defun>\n";

        // Connections
        if (!control_input.empty()) {
            xml << "            <connect-in port=\"" << control_input << "\" place=\"" << control_input << "\"/>\n";
        }
        for (const auto& input : field_inputs) {
            xml << "            <connect-in port=\"" << input << "\" place=\"" << input << "\"/>\n";
        }
        for (const auto& global : global_inputs) {
            if (global == "tailInts" && transition == "T_0") {
                xml << "            <connect-out port=\"" << global << "\" place=\"" << global << "\"/>\n";
            } else {
                xml << "            <connect-read port=\"" << global << "\" place=\"" << global << "\"/>\n";
            }
        }
        for (const auto& output : field_outputs) {
            xml << "            <connect-out port=\"" << output << "\" place=\"" << output << "\"/>\n";
        }

        xml << "        </transition>\n";
    }

    xml << "    </net>\n";
    xml << "</defun>\n";
    return xml.str();
}

// Main function (unchanged from your last version)
int main() {
    // Define target integrals
    std::vector<std::vector<int>> targetInt = {
        {1, 1, 1, -1, -3, -1, -1, -1, -1},
        {1, -1, 1, -1, -3, -1, -1, -4, -1}
    };

    // Generate web (mocked)
    Web web = generateWeb(targetInt);

    // Generate graph data
    GraphData graphData = getGraph(web);

    // Generate DAG
    DAG myDAG = generate_dag(graphData);

    // Find root node
    std::string root_node = find_root_node(myDAG);
    std::cout << "Root Node: " << root_node << "\n";

    // Generate Petri net
    PetriNet myPetriNet = generate_petri_net(myDAG);

    // Generate XML
    std::string xml = generateXPNetXMLWithT0Tend(myPetriNet);
    std::ofstream file("workflow/template.xpnet");
    file << xml;
    file.close();

    return 0;
}
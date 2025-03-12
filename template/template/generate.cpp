#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>

// Define DAG structure
struct DAG {
    std::vector<std::string> vertices;
    std::vector<std::pair<std::string, std::string>> edges;
};

// Define PetriNet structure
struct PetriNet {
    std::vector<std::string> places;
    std::map<std::string, std::string> place_types;
    std::vector<std::string> transitions;
    std::map<std::string, std::vector<std::string>> transition_inputs;
    std::map<std::string, std::vector<std::string>> transition_outputs;
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
    if (!root_nodes.empty()) {
        return *root_nodes.begin();
    } else {
        return "";
    }
}

// Function to generate and print PetriNet from DAG
PetriNet generate_petri_net(const DAG& dag) {
    PetriNet net;
    net.transitions = dag.vertices;

    std::map<std::string, std::vector<std::string>> transition_inputs;
    std::map<std::string, std::string> transition_output;

    std::string root_node = find_root_node(dag);


    // Add input place
    net.places.push_back("input");
    net.place_types["input"] = "string";

    // Create control places and define transitions
    for (const auto& transition : dag.vertices) {
        std::string control_place = "control_" + transition.substr(2);
        net.places.push_back(control_place);
        net.place_types[control_place] = "control";
        transition_inputs[transition].push_back(control_place);
    }

    // Connect root node to P0 and input
    if (!root_node.empty()) {
        transition_inputs[root_node].push_back("input");
    }

    // Assign outputs: Txx -> P_xx
    for (const auto& transition : dag.vertices) {
        std::string place_name = "P_" + transition.substr(2);
        net.places.push_back(place_name);
        net.place_types[place_name] = "string";
        transition_output[transition] = place_name;
    }

    // Connect transitions based on edges
    for (const auto& edge : dag.edges) {
        if (transition_output.find(edge.first) != transition_output.end()) {
            transition_inputs[edge.second].push_back(transition_output[edge.first]);
        }
    }

    for (const auto& transition : dag.vertices) {
        if (transition_output.find(transition) != transition_output.end()) {
            net.transition_outputs[transition] = { transition_output[transition] };
        }
    }

    net.transition_inputs = transition_inputs;
/* 
    // Print PetriNet for debugging
    std::cout << "\nGenerated PetriNet:\n";
    std::cout << "Places:\n";
    for (const auto& place : net.places) {
        std::cout << "  " << place << " (" << net.place_types[place] << ")\n";
    }

    std::cout << "Transitions:\n";
    for (const auto& transition : net.transitions) {
        std::cout << "  " << transition << "\n";
    }

    std::cout << "Transition Inputs:\n";
    for (const auto& [t, inputs] : net.transition_inputs) {
        std::cout << "  " << t << " <- { ";
        for (const auto& i : inputs) std::cout << i << " ";
        std::cout << "}\n";
    }

    std::cout << "Transition Outputs:\n";
    for (const auto& [t, outputs] : net.transition_outputs) {
        std::cout << "  " << t << " -> { ";
        for (const auto& o : outputs) std::cout << o << " ";
        std::cout << "}\n";
    }
 */
    return net;
}

std::string generateXPNetXML(const PetriNet& net, const std::string& root_node) {
    std::ostringstream xml;

    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<defun name=\"template\">\n";
    xml << "    <struct name=\"feynman\">\n";
    xml << "        <field name=\"MI\" type=\"string\"/>\n";
    xml << "        <field name=\"reducedIBPs\" type=\"string\"/>\n";
    xml << "        <field name=\"web\" type=\"string\"/>\n";
    xml << "    </struct>\n";
    xml << "    <in name=\"base_filename\" type=\"string\" place=\"base_filename\"/>\n";
    xml << "    <in name=\"library_name\" type=\"string\" place=\"library_name\"/>\n";
    xml << "    <in name=\"input\" type=\"string\" place=\"input\"/>\n";
    xml << "    <out name=\"output\" type=\"feynman\" place=\"mi_ibp\"/>\n";


    // Find the last output place
    std::string last_output_place = "P0";
    for (const auto& [transition, outputs] : net.transition_outputs) {
        if (!outputs.empty()) {
            last_output_place = outputs[0];
        }
    }

    xml << "    <out name=\"result\" type=\"string\" place=\"" << last_output_place << "\"/>\n";

    xml << "    <net>\n";
    xml << "        <place name=\"data\" type=\"feynman\"/>\n";
    xml << "        <place name=\"base_filename\" type=\"string\"/>\n";
    xml << "        <place name=\"library_name\" type=\"string\"/>\n";
    xml << "        <place name=\"mi_ibp\" type=\"feynman\"/>\n";
    xml << "        <place name=\"input_control\" type=\"control\">\n";
    xml << "            <token>\n                <value>[]</value>\n            </token>\n";
    xml << "        </place>\n";

    xml << "        <transition name=\"compute\">\n";
    xml << "            <defun>\n";
    xml << "                <require key=\"worker\" mandatory=\"true\"/>\n";
    xml << "                <in name=\"input_control\" type=\"control\"/>\n";
    xml << "                <in name=\"library_name\" type=\"string\"/>\n";
    xml << "                <in name=\"base_filename\" type=\"string\"/>\n";
    xml << "                <in name=\"input\" type=\"string\"/>\n";
    xml << "                <out name=\"labeledgraph\" type=\"string\"/>\n";
    xml << "                <out name=\"data\" type=\"feynman\"/>\n";
    xml << "                <module name=\"singular_template\" require_function_unloads_without_rest=\"false\" function=\"compute(input_control,input,library_name,base_filename,labeledgraph,data)\">\n";
    xml << "                    <cinclude href=\"interface/template_interface.hpp\"/>\n";
    xml << "                    <cinclude href=\"util-generic/dynamic_linking.hpp\"/>\n";
    xml << "                    <code><![CDATA[\n";
    xml << "                        labeledgraph = RESOLVE_INTERFACE_FUNCTION(singular_getReducedIBPSystem_gpi)(input, library_name, base_filename);\n";
    xml << "                        data.web = RESOLVE_INTERFACE_FUNCTION(singular_updateWeb_gpi)(input, library_name, base_filename);\n";
    xml << "                        data.reducedIBPs = RESOLVE_INTERFACE_FUNCTION(singular_reducedIBPs_gpi)(input, library_name, base_filename);\n";
    xml << "                        data.MI = RESOLVE_INTERFACE_FUNCTION(singular_MI_gpi)(input, library_name, base_filename);\n";
    xml << "                        std::cout << \" web \" << data.web << std::endl;\n";
    xml << "                    ]]></code>\n";
    xml << "                </module>\n";
    xml << "            </defun>\n";
    xml << "            <connect-read port=\"library_name\" place=\"library_name\"/>\n";
    xml << "            <connect-read port=\"base_filename\" place=\"base_filename\"/>\n";
    xml << "            <connect-read port=\"input\" place=\"input\"/>\n";
    xml << "            <connect-in port=\"input_control\" place=\"input_control\"/>\n";
    xml << "            <connect-out port=\"labeledgraph\" place=\"labeledgraph\"/>\n";
    xml << "            <connect-out port=\"data\" place=\"data\"/>\n";
    xml << "        </transition>\n";   
    xml << "        <place name=\"labeledgraph\" type=\"string\"/>\n";
    // Count how many times each place is used as an input
    std::map<std::string, int> place_usage_count;
    for (const auto& [transition, inputs] : net.transition_inputs) {
        for (const auto& in_place : inputs) {
            place_usage_count[in_place]++;
        }
    }

    // Declare places
    std::set<std::string> declared_places;
    for (const auto& place : net.places) {
        if (declared_places.find(place) == declared_places.end()) {
            std::string type = net.place_types.at(place);
            if (type == "string") {
                xml << "        <place name=\"" << place << "\" type=\"" << type << "\"/>\n";
            }
            else if (type == "control") {
                xml << "        <place name=\"" << place << "\" type=\"" << type << "\">\n";
                xml << "            <token>\n                <value>[]</value>\n            </token>\n";
                xml << "        </place>\n";
            }
            declared_places.insert(place);
        }
    }

    // Declare transitions
    for (const auto& transition : net.transitions) {
        xml << "        <transition name=\"" << transition << "\">\n";
        xml << "            <defun>\n";
        xml << "                <require key=\"worker\" mandatory=\"true\"/>\n";
        xml << "                <in name=\"library_name\" type=\"string\"/>\n";
        xml << "                <in name=\"base_filename\" type=\"string\"/>\n";
        std::vector<std::string> function_args;
        std::set<std::string> declared_inputs;

        if (net.transition_inputs.find(transition) != net.transition_inputs.end()) {
            for (const auto& in_place : net.transition_inputs.at(transition)) {
                std::string type = (in_place.find("control_") == 0) ? "control" : "string";
                if (declared_inputs.find(in_place) == declared_inputs.end()) {
                    xml << "                <in name=\"" << in_place << "\" type=\"" << type << "\"/>\n";
                    function_args.push_back(in_place);
                    declared_inputs.insert(in_place);
                }
            }
        }

        if (net.transition_outputs.find(transition) != net.transition_outputs.end() && !net.transition_outputs.at(transition).empty()) {
            std::string out_place = net.transition_outputs.at(transition)[0];
            xml << "                <out name=\"" << out_place << "\" type=\"string\"/>\n";
            function_args.push_back(out_place);
        }
        xml << "                <module name=\"singular_template\" require_function_unloads_without_rest=\"false\" function=\"f_" << transition << " (";
        for (size_t i = 0; i < function_args.size(); ++i) {
            xml << function_args[i];
            if (i < function_args.size() - 1) {
                xml << ", ";
            }
        }
        xml << ", library_name, base_filename";
        xml << ")\">\n";

        if (transition == root_node) {
            xml << "              <cinclude href=\"interface/template_interface.hpp\"/> \n";
            xml << "              <cinclude href=\"util-generic/dynamic_linking.hpp\"/> \n";
            xml << "                    <code><![CDATA[\n";
            xml << "                        std::cout << \"DEBUG: Transition " << transition << " starting.\" << std::endl;\n";
            xml << "                        int res = h(4, 5);\n";
            xml << "                        std::cout << \"T11: Result = \" << res << std::endl;\n";
            if (!function_args.empty()) {
                std::string out_place = function_args.back();
                xml << "                        " << out_place << " = std::to_string(res);\n";
                xml << "                        std::cout << \"DEBUG: " << transition << " output assigned to " << out_place << " with value \" << res << std::endl;\n";
            } else {
                xml << "                        std::cerr << \"ERROR: No output place defined for transition " << transition << "!\" << std::endl;\n";
            }
            xml << "                        std::cout << \"DEBUG: Transition " << transition << " finished.\" << std::endl;\n";
            xml << "                    ]]></code>\n";
        }
        else {
            xml << "              <cinclude href=\"interface/template_interface.hpp\"/> \n";
            xml << "              <cinclude href=\"util-generic/dynamic_linking.hpp\"/> \n";
            xml << "                    <code><![CDATA[\n";
            xml << "                        std::cout << \"DEBUG: Transition " << transition << " starting.\" << std::endl;\n";
            xml << "                        std::cout << \"DEBUG: function_args[1] = '\" << " << function_args[1] << " << \"'\" << std::endl;\n";
            xml << "                        int input = std::stoi(" << function_args[1] << ");\n";
            xml << "                        std::string output_value;\n";
            xml << "                        int result = g(input, output_value);\n";
            xml << "                        std::cout << \"DEBUG: Transition " << transition << " result = \" << result << std::endl;\n";
            if (!function_args.empty()) {
                std::string out_place = function_args.back();
                xml << "                        " << out_place << " = std::to_string(result);\n";
                xml << "                        std::cout << \"DEBUG: " << transition << " output assigned to " << out_place << " with value \" << result << std::endl;\n";
            } else {
                xml << "                        std::cerr << \"ERROR: No output place defined for transition " << transition << "!\" << std::endl;\n";
            }
            xml << "                    ]]></code>\n";
        }
        xml << "                </module>\n";
        xml << "            </defun>\n";

        // Connect inputs and outputs
        std::set<std::string> connected_inputs;
        std::set<std::string> connected_outputs;
        if (net.transition_inputs.find(transition) != net.transition_inputs.end()) {
            for (const auto& in_place : net.transition_inputs.at(transition)) {
                if (connected_inputs.find(in_place) == connected_inputs.end()) {
                    if (place_usage_count[in_place] > 1) {
                        xml << "            <connect-read port=\"" << in_place << "\" place=\"" << in_place << "\"/>\n";
                    } else {
                        xml << "            <connect-in port=\"" << in_place << "\" place=\"" << in_place << "\"/>\n";
                    }
                    connected_inputs.insert(in_place);
                }
            }
        }

        if (net.transition_outputs.find(transition) != net.transition_outputs.end() &&
            !net.transition_outputs.at(transition).empty()) {
            std::string out_place = net.transition_outputs.at(transition)[0];
            if (connected_outputs.find(out_place) == connected_outputs.end()) {
                xml << "            <connect-out port=\"" << out_place << "\" place=\"" << out_place << "\"/>\n";
                connected_outputs.insert(out_place);
            }
        }

        // Updated connect-read statements for global inputs.
        xml << "        <connect-read port=\"library_name\" place=\"library_name\"/>\n";
        xml << "        <connect-read port=\"base_filename\" place=\"base_filename\"/>\n";
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

    std::string xml = generateXPNetXML(myPetriNet, root_node);

    std::ofstream file("workflow/template.xpnet");
    file << xml;
    file.close();

    return 0;
}
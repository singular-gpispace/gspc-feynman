#include <interface/Workflow.hpp>

#include <iostream>
#include "template/config.hpp"
#include <Singular/libsingular.h>


namespace template_module
{
    ParametersDescription Workflow::options()
    {
        namespace po = boost::program_options;

        ParametersDescription workflow_opts("Workflow");

        workflow_opts.add_options()("basefilename", po::value<std::string>()->required());
        workflow_opts.add_options()("input", po::value<std::string>()->required());
        workflow_opts.add_options()("libraryname", po::value<std::string>()->required());

        return workflow_opts;
    }

    Workflow::Workflow(Parameters const& args)
        :_input(args.at("input").as<std::string>())
        , _basefilename(args.at("basefilename").as<std::string>())
        , _libraryname(args.at("libraryname").as<std::string>())
    {
    }

    ValuesOnPorts Workflow::inputs() const
    {
        ValuesOnPorts::Map values_on_ports;

        values_on_ports.emplace("input", _basefilename + _input);
        values_on_ports.emplace("library_name", _libraryname);
        values_on_ports.emplace("base_filename", _basefilename);

        return values_on_ports;
    }

void printListElements(lists output) {
    if (!output) {
        std::cerr << "ERROR: Null list in printListElements" << std::endl;
        return;
    }
    
    std::cout << "List with " << output->nr + 1 << " elements:" << std::endl;
    for (int i = 0; i <= output->nr; i++) {
        std::cout << "  [" << i << "]: ";
        if (output->m[i].rtyp == STRING_CMD) {
            std::cout << "STRING: " << (char*)output->m[i].data << std::endl;
        } else if (output->m[i].rtyp == INT_CMD) {
            std::cout << "INT: " << (int)(long)output->m[i].data << std::endl;
        } else if (output->m[i].rtyp == LIST_CMD) {
            std::cout << "LIST" << std::endl;
            printListElements((lists)output->m[i].data);
        } else if (output->m[i].rtyp == RING_CMD) {
            std::cout << "RING: " << rString((ring)output->m[i].data) << std::endl;
        } else if (output->m[i].rtyp == MATRIX_CMD) {
            std::cout << "MATRIX" << std::endl;
            //printMat((matrix)output->m[i].data);
            std::cout << "this is a matrix" << std::endl;
        } else {
            std::cout << "Type: " << output->m[i].rtyp << std::endl;
        }
    }
}
    void Workflow::process(WorkflowResult const& results, [[maybe_unused]] Parameters const& parameters, leftv res) const {
        // Allocate and initialize the output list
        lists out_list = static_cast<lists>(omAlloc0Bin(slists_bin));

        // Retrieve the map from the results
        auto& valuesOnPortsMap = results.template_module::ValuesOnPorts::map();

        // Get the output value
        auto output_it = valuesOnPortsMap.find("output");
        if (output_it != valuesOnPortsMap.end()) {
            try {
                // Get the output string
                std::string output = boost::get<std::string>(output_it->second);
                //std::cout << "Raw output: " << output << std::endl;

                // Split the output string by semicolons
                std::vector<std::string> parts;
                size_t start = 0;
                size_t end = 0;
                while ((end = output.find(';', start)) != std::string::npos) {
                    parts.push_back(output.substr(start, end - start));
                    start = end + 1;
                }
                if (start < output.length()) {
                    parts.push_back(output.substr(start));
                }

                // Initialize the list with the number of parts
                out_list->Init(parts.size());
                //std::cout << "Initialized out_list with size: " << parts.size() << std::endl;

                // Add each part to the list
                for (size_t i = 0; i < parts.size(); ++i) {
                    out_list->m[i].rtyp = STRING_CMD;
                    out_list->m[i].data = strdup(parts[i].c_str());
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error processing output: " << e.what() << std::endl;
                out_list->Init(1);
                out_list->m[0].rtyp = STRING_CMD;
                out_list->m[0].data = strdup("Error processing output");
            }
        } else {
            std::cerr << "No output found in results" << std::endl;
            out_list->Init(1);
            out_list->m[0].rtyp = STRING_CMD;
            out_list->m[0].data = strdup("No output found");
        }

        // Set the result type and data
        res->rtyp = LIST_CMD;
        res->data = out_list;
        //print the result
        printListElements(out_list);
    }
}
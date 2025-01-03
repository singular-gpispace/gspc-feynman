#include <interface/Workflow.hpp>

#include <iostream>


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



    void Workflow::process(WorkflowResult const& results, [[maybe_unused]] Parameters const& parameters, leftv res) const {
        // Allocate and initialize the output list
        lists out_list = static_cast<lists>(omAlloc0Bin(slists_bin));

        // Retrieve the map from the results
        auto& valuesOnPortsMap = results.template_module::ValuesOnPorts::map();

        // Determine the number of "OUTPUT" entries
        int output_count = 0;
        for (auto it = valuesOnPortsMap.begin(); it != valuesOnPortsMap.end(); ++it) {
            if (boost::get<std::string>(it->first) == "place_output") {
                output_count++;
            }
        }

        //std::cout << "output count: " << output_count << std::endl;

        // Initialize the list with the number of "OUTPUT" entries
        out_list->Init(output_count);
        //std::cout << "Initialized out_list with size: " << output_count << std::endl;

        int i = 0;

        for (auto it = valuesOnPortsMap.begin(); it != valuesOnPortsMap.end(); ++it) {
            if (boost::get<std::string>(it->first) == "place_output") {
                try {
                    // Extract and print the raw input
                    std::string raw_data = boost::get<std::string>(it->second);
                    //std::cout << "Raw data: " << raw_data << std::endl;

                    // Skip deserialization and use raw data directly
                    // std::cout << "Skipping deserialization for debug purposes.\n";
                    out_list->m[i].rtyp = STRING_CMD;  // Assuming it's a string type
                    out_list->m[i].data = strdup(raw_data.c_str());  // Copy raw string
                    i++;
                }
                catch (const std::exception& e) {
                    std::cerr << "Error accessing data: " << e.what() << std::endl;
                }
            }
        }


        // Verify that the number of entries processed matches the expected count
        if (i != output_count) {
            std::cerr << "Warning: Processed " << i << " OUTPUT entries, but expected " << output_count << std::endl;
        }

        // Set the result type and data
        res->rtyp = LIST_CMD;
        res->data = out_list;
    }
}
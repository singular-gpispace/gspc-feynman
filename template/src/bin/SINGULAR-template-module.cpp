#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <installation.hpp>

#include <util-generic/executable_path.hpp>
#include <util-generic/print_exception.hpp>
#include <util-generic/read_lines.hpp>

#include "Singular/libsingular.h"
#include <template/singular_functions.hpp>

#include <interface/parse_parameters_from_singular.hpp>
#include <interface/execute.hpp>
#include <interface/Parameters.hpp>
#include <interface/Workflow.hpp>

void sggspc_print_current_exception(std::string s)
{
    WerrorS(("singular_template: (" + s + ") " +
        fhg::util::current_exception_printer(": ").string()).c_str());
}

BOOLEAN sggspc_template(leftv res, leftv args)
try {
    std::string command_opts = reinterpret_cast<char*> (args->Data());

    // (1) loading configuration options
    auto const parameters = template_module::parse_parameters_from_singular
    (template_module::execution::options(),
        template_module::Workflow::options(),
        command_opts
    );
    //std::cout << "Parameters loaded successfully." << std::endl;
   // (2) initializing a workflow
    template_module::Workflow const workflow(parameters);
    //std::cout << "Workflow initialized successfully." << std::endl;

    // (3) executing the workflow
    auto const results = template_module::execute(parameters, workflow);
    //std::cout << "Workflow executed. Results obtained." << std::endl;
    // (4) evaluating the workflow result
    workflow.process(results, parameters, res);
    //std::cout << "workflow.process. process obtained." << std::endl;
    return FALSE;
}
catch (...)
{
    // need to check which resources must be tidied up
    sggspc_print_current_exception(std::string("in sggspc_template"));
    return TRUE;
}

extern "C" int mod_init(SModulFunctions* psModulFunctions)
{
    // TODO: explicit check if module has already been loaded?
    //PrintS ("DEBUG: in mod_init\n");

    psModulFunctions->iiAddCproc
    ((currPack->libname ? currPack->libname : ""),
        "sggspc_template", FALSE, sggspc_template);

    return MAX_TOK;
}
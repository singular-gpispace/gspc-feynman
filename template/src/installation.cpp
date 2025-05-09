#include <installation.hpp>
#include <util-generic/executable_path.hpp>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <cstdlib> // for std::getenv

namespace singular_template
{
  namespace
  {
    std::filesystem::path gspc_home(const std::filesystem::path& gspc_path)
    {
      if (gspc_path.empty())
      {
        const char* env_gspc_home = std::getenv("GSPC_HOME");
        if (!env_gspc_home || std::string(env_gspc_home).empty())
        {
          throw std::runtime_error("GSPC_HOME environment variable not set or empty");
        }
        return std::filesystem::canonical(env_gspc_home);
      }
      return std::filesystem::canonical(gspc_path);
    }

    std::filesystem::path workflow_path(const std::filesystem::path& installation_path)
    {
      if (installation_path.empty())
      {
        throw std::runtime_error("Installation path is empty");
      }
      return installation_path / "libexec" / "workflow";
    }

    std::filesystem::path workflow_all_file(const std::filesystem::path& installation_path)
    {
      auto path = workflow_path(installation_path) / "template.pnet";
      if (!std::filesystem::exists(path))
      {
        throw std::runtime_error("Workflow file not found: " + path.string());
      }
      return path;
    }
  }

  installation::installation()
  {
    const char* install_dir = std::getenv("GSPC_FEYNMAN_INSTALL_DIR");
    if (!install_dir || std::string(install_dir).empty())
    {
      throw std::runtime_error("GSPC_FEYNMAN_INSTALL_DIR environment variable not set or empty");
    }
    _path = std::filesystem::canonical(install_dir);
    _gspc_path = gspc_home(std::filesystem::path());
  }

  installation::installation(const std::filesystem::path& ip)
  {
    _path = std::filesystem::canonical(ip);
    _gspc_path = gspc_home(std::filesystem::path());
  }

  std::filesystem::path installation::workflow_all() const
  {
    return workflow_all_file(_path);
  }

  std::filesystem::path installation::workflow_dir() const
  {
    return workflow_path(_path);
  }

  gspc::installation installation::gspc_installation(boost::program_options::variables_map& vm) const
  {
    gspc::set_gspc_home(vm, gspc_home(_gspc_path));
    gspc::set_application_search_path(vm, workflow_path(_path));
    return gspc::installation(vm);
  }
}
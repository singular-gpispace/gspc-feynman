#include <installation.hpp>
#include <util-generic/executable_path.hpp>
#include <filesystem>
#include <stdexcept>

namespace singular_template
{
  const std::filesystem::path SP_INSTALL_PATH = "/home/atraore/gpi/try_gpi/feynman_ibp/install_dir";

  namespace
  {
    void check(std::filesystem::path const& path, bool condition, std::string const& message)
    {
      if (!condition)
      {
        throw std::logic_error(path.string() + " " + message + ": Installation incomplete!?");
      }
    }

    void check_is_directory(std::filesystem::path const& path)
    {
      check(path, std::filesystem::is_directory(path), "is not a directory");
    }

    void check_is_file(std::filesystem::path const& path)
    {
      check(path, std::filesystem::exists(path), "does not exist");
      check(path, std::filesystem::is_regular_file(path), "is not a regular file");
    }

    std::filesystem::path gspc_home(std::filesystem::path const& gspc_path)
    {
      return gspc_path;
    }

    std::filesystem::path workflow_path(std::filesystem::path const& installation_path)
    {
      return installation_path / "libexec" / "workflow";
    }

    std::filesystem::path workflow_all_file(std::filesystem::path const& installation_path)
    {
      return workflow_path(installation_path) / "template.pnet";
    }
  }

  installation::installation()
    : installation(SP_INSTALL_PATH) 
  {
  }

  installation::installation(std::filesystem::path const& ip)
    : installation(ip, std::filesystem::path(fhg::util::executable_path().parent_path().parent_path().string()))
  {
  }

  installation::installation(std::filesystem::path const& ip, std::filesystem::path const& gp)
    : _path(ip), _gspc_path(gp)
  {
    check_is_directory(gspc_home(_gspc_path));
    check_is_directory(workflow_path(_path));
    check_is_file(workflow_all());
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

    return { vm };
  }
}

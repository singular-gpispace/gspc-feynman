#ifndef INSTALLATION_HPP
#define INSTALLATION_HPP
#include <drts/drts.hpp>

#include <filesystem>
#include <boost/program_options.hpp>

namespace singular_template
{
  class installation
  {
  public:
    installation();
    installation(const std::filesystem::path&);
    installation(const std::filesystem::path&, const std::filesystem::path&);

    std::filesystem::path workflow_all() const;
    std::filesystem::path workflow_dir() const;
    
    gspc::installation gspc_installation(boost::program_options::variables_map&) const;  // ✅ Ensure gspc is recognized

  private:
    std::filesystem::path _path;
    std::filesystem::path _gspc_path;
  };
}

#endif // INSTALLATION_HPP

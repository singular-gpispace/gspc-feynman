# Copyright 2013-2021 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack import *


class GspcFeynman(CMakePackage):
    """FIXME: Put a proper description of your package here."""

    homepage = "https://github.com/singular-gpispace/gspc-feynman"
    git      = "https://github.com/singular-gpispace/gspc-feynman"

    maintainers = ['alitraore','jankoboehm']

    version('latest', branch='main')

    depends_on('singular@4.4.0p2')
    depends_on('gpi-space@24.12:')
    depends_on('flint@2.6.3:')
    depends_on('boost@1.63.0: +serialization +system +filesystem +program_options +thread +chrono +date_time +atomic')

    def cmake_args(self):
        spec = self.spec
        print(self.spec)
        args = [ self.define("GSPC_HOME", spec['gpi-space'].prefix)
               , self.define("SINGULAR_HOME", spec['singular'].prefix)
               , self.define("FLINT_HOME", spec['flint'].prefix)
               ]
        return args

    @property
    def root_cmakelists_dir(self):
        return self.stage.source_path + "/template"

    def setup_run_environment(self, env):
        spec = self.spec
        env.set('SINGULAR_INSTALL_DIR', spec['singular'].prefix)
        env.set('GSPC_FEYNMAN_INSTALL_DIR', self.prefix)
        env.set('GSPC_FEYNMAN_EXAMPLES_DIR', self.prefix.share.examples)

    def setup_dependent_run_environment(self, env, dependent_spec):
        env.set('GSPC_FEYNMAN_EXAMPLES_DIR', self.prefix.share.examples)


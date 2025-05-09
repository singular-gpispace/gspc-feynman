# Copyright 2013-2021 Lawrence Livermore National Security, LLC
# and other Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack import *
import os


class GspcFeynman(CMakePackage):
    """A GPI-Space based implementation for computing Feynman integrals.
    
    This package provides parallel computation capabilities for Feynman integrals
    using GPI-Space and Singular. It includes examples demonstrating the usage
    of the Feynman module for various computations."""

    homepage = "https://github.com/singular-gpispace/gspc-feynman"
    git      = "https://github.com/singular-gpispace/gspc-feynman"

    maintainers = ['alitraore', 'jankoboehm']

    version('latest', branch='main')

    depends_on('singular@4.4.0p2')
    depends_on('gpi-space@24.12:')
    depends_on('flint@2.6.3:')
    depends_on('boost@1.63.0: +serialization +system +filesystem +program_options +thread +chrono +date_time +atomic')

    @property
    def root_cmakelists_dir(self):
        return self.stage.source_path + "/template"

    def cmake_args(self):
        spec = self.spec
        return [
            self.define("GSPC_HOME", spec['gpi-space'].prefix),
            self.define("SINGULAR_HOME", spec['singular'].prefix),
            self.define("FLINT_HOME", spec['flint'].prefix)
        ]

    def setup_run_environment(self, env):
        """Set environment variables when the package is loaded."""
        env.set('SINGULAR_INSTALL_DIR', self.spec['singular'].prefix)
        env.set('GSPC_FEYNMAN_INSTALL_DIR', self.prefix)
        env.set('GSPC_FEYNMAN_EXAMPLES_DIR', self.prefix.share.examples)

    def setup_dependent_run_environment(self, env, dependent_spec):
        """Set environment variables for packages depending on this one."""
        env.set('GSPC_FEYNMAN_EXAMPLES_DIR', self.prefix.share.examples)

    @run_after('install')
    def install_examples(self):
        """Install example files and print final instructions."""
        examples_dir = os.path.join(self.prefix.share, 'examples')
        mkdirp(examples_dir)
        install('example_dir/templategp.lib', examples_dir)
        install('example_dir/templategspc.lib', examples_dir)

        # Post-install instructions
        print("=" * 80)
        print("GSPC-FEYNMAN INSTALLATION COMPLETED SUCCESSFULLY!")
        print("=" * 80)
        print("Installation Details:")
        print("-" * 40)
        print(f"Examples are installed in: {examples_dir}")
        print("Required Environment Variables:")
        print("-" * 40)
        print(f"export SINGULAR_INSTALL_DIR={self.spec['singular'].prefix}")
        print(f"export GSPC_FEYNMAN_INSTALL_DIR={self.prefix}")
        print(f"export GSPC_FEYNMAN_EXAMPLES_DIR={examples_dir}")
        print("=" * 80)
        print("To try an example:")
        print(f"  singular {examples_dir}/templategp.lib")
        print("=" * 80)

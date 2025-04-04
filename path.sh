singular_path=/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-snapshot_22_03-5jvwtprazqirywu2triw6rprjazzi3so/bin/Singular
mysingular=$singular_path
$mysingular


LD_LIBRARY_PATH=/home/atraore/git/Singular4/flint/flint2:$LD_LIBRARY_PATH ~/Singular4/bin/Singular


[ -f /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt ] && rm /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt; cd ~/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-23.06-gxye6b7ngsnbxnzjkfsfqtvanynyghdk/bin && ./gspc-logging-to-stdout.exe --port 9876 >> /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt 2>&1


LD_LIBRARY_PATH=/home/atraore/git/Singular4/flint/flint2:$LD_LIBRARY_PATH ~/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-4bgipnkqrvp6anrqew3a5a46go5vj7kh/bin/Singular

gspc-monitor --port 9876 &
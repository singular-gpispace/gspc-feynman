singular_path=/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-snapshot_22_03-5jvwtprazqirywu2triw6rprjazzi3so/bin/Singular
mysingular=$singular_path
$mysingular



[ -f /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt ] && rm /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt; cd ~/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-23.06-gxye6b7ngsnbxnzjkfsfqtvanynyghdk/bin && ./gspc-logging-to-stdout.exe --port 9876 >> /home/atraore/gpi/try_gpi/feynman_ibp/monitor.txt 2>&1

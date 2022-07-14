# run mpi program
# print first argument name of program

# put to variable all arguments from 2 to last
# $@ - all arguments
# $1 - first argument
# $2 - second argument
# ...
# $n - n-th argument    
# $# - number of arguments  
# $? - exit code of last command
# $0 - name of script

programName=$1
shift # remove first argument from $@
arguments=$@
# copy main to home
cp $programName /home/mpiuser

#change user to mpiuser
whoami
sudo -i -u mpiuser bash << EOF
echo "In"
whoami
# copy file to other machine
echo "Copy file to other machine"
scp ./$programName acer:$programName
echo "Running mpi program" $programName
cd 
mpiexec -n 4 -hosts dell,acer ./$programName $arguments
EOF
echo "Out"
whoami

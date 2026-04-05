# ./build.sh
echo ""
echo "~~~~~~~~RUNNING ON REMOTE~~~~~~~~"
echo ""

# SETUP_PATH='/home/ppartha/IMR_Testing/setup'
sshpass -p 'viz@seteam' ssh -t ppartha@107.108.71.17 'mkdir -p /home/ppartha/LOD_Testing/setup'

echo "Transfering the setup..."
sshpass -p 'viz@seteam' scp -r * ppartha@107.108.71.17:/home/ppartha/LOD_Testing/setup

echo "Running the setup..."
sshpass -p 'viz@seteam' ssh -t ppartha@107.108.71.17 'cd /home/ppartha/LOD_Testing/setup; ./run.sh'

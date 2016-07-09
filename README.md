Course Project for CS 224(Computer Networks)

This is a client-server system in C/C++ used for password cracking using distributed computing.

Implementation Details:
At a high level, this project consist of 3 programs: User, Server and
Worker. A user sends a hash of a password to the server for cracking it. The server will divide the job
among a group of workers, who get back to the server with the status of the allocated job. After
completion of the job, the server gets back to the user with the password.

1. User: A user passes a hash to the server and will print out the password once it hears back from
the server as well as the time taken for cracking the password. Note server may take a long time
in getting back, your code should work given this.
A User passes three pieces of information to the server
◦ hash of the password
◦ no of characters in the password
◦ a 3 bit binary string, where the three bits indicate the use of lower case, upper case and
numerical characters in the password. For example, 001 indicates the password is made up of
only numbers; 111 indicates the password can contain lower, upper as well as numerical
characters
Your user code should produce an executable 'user' and should use the following arguments:
./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>
Example: ./user osl-9.cse.iitb.ac.in 5000 aBBHPfIg 6 001
The user should also print the password and time taken for cracking the password on the console
once it hears back from the server.

2. Server: The server can be contacted by multiple users and workers at a given port (use Select call
to handle this). In this lab, we will restrict maximum number of users to 3 and workers to 5. The
server can allocate jobs as it sees fit. The logic is yours but ensure that the load is balanced and it
does an efficient job. For this, the server needs to keep track of who is working on which job and
the current status. As a new user requests come, server should assign these jobs to workers
without prempting them.
Your server code should produce an executable 'server' and should use the following arguments:
./server <server-port>
Example: ./server 5000 (basically server is listening on port 5000)

3. Worker: A worker registers with the server as soon as it is started. Then it waits for a job from
the server and gets back to the server with a status of the job once it completes it.
Your worker code should produce an executable 'worker' and should use the following
arguments:
./worker server ip/host-name> <server-port>
Example: ./worker osl-9.cse.iitb.ac.in 5000

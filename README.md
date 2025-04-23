# ReverseShell
Simple Reverse Shell implementation in C++


![reverse_shell](https://github.com/user-attachments/assets/403be26d-e151-4d13-b755-67b86d740533)

I made this reverse shell in c++ as a cybersecurity exercise. It works pretty well and is really simple and I might add some more features later. To test it I made two ubuntu 24.04 Virtual Machines and put them on an isolated network with a point to point connection. The victim's machine was set to 10.0.0.100 and the attacking server was set to 10.0.0.101. 

The client would start a process that can connect to the server's IP and then it listens for commands to execute. The server executes those commands and then recieves the output directly without the client seeing anything. This method of leaving a backdoor is effective because outbound connections are generally less regulated than inbound connections in firewalls - especially Windows Defender - and it's easier for a victim to connect to a server than a server to the client. This method is quite easy to detect though as traffic monitoring softwares will likely show the traffic or flag it as unusual. Additionally, it opens a connection to the attacker's server making the attacker vulnerable as well.

To use this, start the server on the attacker's machine. Then, start the client on the victim's machine and point the IP to the attacker's IP. 

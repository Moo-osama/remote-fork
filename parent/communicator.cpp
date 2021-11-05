#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <experimental/filesystem>
#include <fstream>
#include <iostream>

using namespace std;
void fork_me();

class Server_socket
{
  fstream file;

  int PORT;

  int general_socket_descriptor;
  int new_socket_descriptor;

  struct sockaddr_in address;
  int address_length;

  public:
  Server_socket()
  {
    create_socket();
    PORT = 8050;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    address_length = sizeof(address);

    bind_socket();
    set_listen_set();
    accept_connection();
  }

  void create_socket()
  {
    if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("[ERROR] : Socket failed");
      exit(EXIT_FAILURE);
    }
    cout << "[LOG] : Socket Created Successfully.\n";
  }

  void bind_socket()
  {
    if (bind(general_socket_descriptor, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
      perror("[ERROR] : Bind failed");
      exit(EXIT_FAILURE);
    }
    cout << "[LOG] : Bind Successful.\n";
  }

  void set_listen_set()
  {
    if (listen(general_socket_descriptor, 3) < 0) {
      perror("[ERROR] : Listen");
      exit(EXIT_FAILURE);
    }
    cout << "[LOG] : Socket in Listen State (Max Connection Queue: 3)\n";
  }

  void accept_connection()
  {
    if ((new_socket_descriptor =
           accept(general_socket_descriptor, (struct sockaddr *)&address,
                  (socklen_t *)&address_length)) < 0) {
      perror("[ERROR] : Accept");
      exit(EXIT_FAILURE);
    }
    cout << "[LOG] : Connected to Client.\n";
  }

  void send_signal()
  {
    string contents = "hello";
    int bytes_sent =
      send(new_socket_descriptor, contents.c_str(), contents.length(), 0);
    cout << "[LOG] : Transmitted Data Size " << bytes_sent << " Bytes.\n";

    cout << "[LOG] : File Transfer Complete.\n";
  }
  int transmit_file()
  {
    FILE *myfile;
    if (myfile = fopen("to_send/image.dmtcp", "r")) {
      fclose(myfile);

      file.open("to_send/image.dmtcp", ios::in | ios::binary);

      if (file.is_open()) {
        cout << "[LOG] : File is ready to Transmit.\n";
      } else {
        cout << "[ERROR] : File loading failed, Exititng.\n";
        exit(EXIT_FAILURE);
      }
      std::string contents((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
      cout << "[LOG] : Transmission Data Size " << contents.length()
           << " Bytes.\n";

      cout << "[LOG] : Sending...\n";

      int bytes_sent =
        send(new_socket_descriptor, contents.c_str(), contents.length(), 0);
      cout << "[LOG] : Transmitted Data Size " << bytes_sent << " Bytes.\n";

      cout << "[LOG] : File Transfer Complete.\n";
      file.close();
      remove("to_send/image.dmtcp");
      //close(new_socket_descriptor);
      return 1;
    } else {
      // printf("No file to send!\n");
      return 0;
    }
  }
};

class Client_socket
{
  fstream file;

  int PORT;
  int general_socket_descriptor;

  struct sockaddr_in address;
  int address_length;

  public:
  Client_socket()
  {
    create_socket();
    PORT = 8060;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address_length = sizeof(address);
    if (inet_pton(AF_INET, "172.17.0.3", &address.sin_addr) <= 0) { //172.17.0.3
      cout << "[ERROR] : Invalid address\n";
    }

    create_connection();
  }

  void create_socket()
  {
    if ((general_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("[ERROR] : Socket failed.\n");
      exit(EXIT_FAILURE);
    }
    cout << "[LOG] : Socket Created Successfully.\n";
  }

  void create_connection()
  {
    while (connect(general_socket_descriptor, (struct sockaddr *)&address,
                   sizeof(address)) < 0) {
      perror("[ERROR] : connection attempt failed.\n");
      // exit(EXIT_FAILURE);
      sleep(1);
    }
    fcntl(general_socket_descriptor, F_SETFL, O_NONBLOCK);
    cout << "[LOG] : Connection Successfull.\n";
  }

  void receive_signal()
  {
    char buffer[1024] = {};
    int valread = read(general_socket_descriptor, buffer, 1024);
    while (valread <= 0) {
      cout << "[LOG] : Data received " << valread << " bytes\n";
      int valread = read(general_socket_descriptor, buffer, 1024);
    }
  }

  int receive_file()
  {
    char buffer[1024];

    fd_set set;
    struct timeval timeout;
    int rv;

    FD_ZERO(&set); /* clear the set */
    FD_SET(general_socket_descriptor,
           &set); /* add our file descriptor to the set */

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    rv = select(general_socket_descriptor + 1, &set, NULL, NULL, &timeout);
    if (rv == -1) {
      perror("select"); /* an error accured */
      return -1;
    } else if (rv == 0) {
      // cout << "No file to receive\n";
      return 0;
    }

    else {
      string fname = "image.dmtcp";
      int fd;
      fd = open(fname.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

      char buffer[1024];

      int valread = read(general_socket_descriptor, buffer, 1024);

      if (valread > 0) {
        cout << valread << endl;

        write(fd, buffer, valread);
        int nb = valread;

        do {
          nb += valread;
          valread = read(general_socket_descriptor, buffer, 1024);
          write(fd, buffer, valread);

        } while (valread > 0);

        cout << "[LOG] : Data received " << nb << " bytes\n";

        cout << "[LOG] : File Saved.\n";

        fork_me();

        return 1;

      } else {
        // cout << "No file to receive\n";

        return 0;
      }
    }
  }
};

void
handshake(Server_socket &server, Client_socket &client)
{
  server.send_signal();
  sleep(1);
  client.receive_signal();
  printf("Handshaking done!\n");
}

int
main()
{
  Server_socket server;
  Client_socket client;

  while (1) {
    server.transmit_file();
    sleep(5);
    client.receive_file();
    sleep(5);
  }

  return 0;
}


void
fork_me()
{
  pid_t pid;
  int ret = 1;
  int status;
  pid = fork();

  if (pid == -1) {
    // pid == -1 means error occured
    printf("can't fork, error occured\n");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("child process, pid = %u\n", getpid());
    printf("parent of child process, pid = %u\n", getppid());

    char *cmd = "dmtcp_restart";
    char *argv[3];
    argv[0] = "dmtcp_restart";
    argv[1] = "--join";
    argv[2] = "image.dmtcp";
    argv[3] = NULL;

    execvp(cmd, argv);

    printf("Return not expected. Must be an execv error.n\n");
    exit(0);
  } else {
    printf("Parent Of parent process, pid = %u\n", getppid());
    printf("parent process, pid = %u\n", getpid());

    if (waitpid(pid, &status, 0) > 0) {
      if (WIFEXITED(status) && !WEXITSTATUS(status))
        printf("program execution successful\n");

      else if (WIFEXITED(status) && WEXITSTATUS(status)) {
        if (WEXITSTATUS(status) == 127) {
          // execv failed
          printf("execv failed\n");
        } else
          printf("program terminated normally,"
                 " but returned a non-zero status\n");
      } else
        printf("program didn't terminate normally\n");
    } else {
      printf("waitpid() failed\n");
    }
  }
}
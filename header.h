#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <locale>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <algorithm>
#include <string.h>

#define SERVER_IP "10.241.107.115"
#define NUMBER_PORT 1614
#define MAX_SIZE_BUF 10000

using namespace std;

void ErrorOrNo(int meaning);
void Communication_With_Client(int server);

#include <utility>

const char* requestFifoName = "/tmp/scissorpaperrockRequest";
const char* responseFifoName = "/tmp/scissorpaperrockResponse";

struct Session;
struct Msg;

// struct FifoMsg
// {
//     Session* session;
//     Msg msg;
// };

void sendFifoMsg(int fifofd, Session* session, Msg* msg);
std::pair<Session*, Msg*> decodeFifoMsg(int fifofd);
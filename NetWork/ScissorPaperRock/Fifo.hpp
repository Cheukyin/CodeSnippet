#ifndef _SCISSORPAPERROCK_FIFO_
#define _SCISSORPAPERROCK_FIFO_

#include <utility>

static const char* requestFifoName = "/tmp/scissorpaperrockRequest";
static const char* responseFifoName = "/tmp/scissorpaperrockResponse";

struct Session;
struct Msg;

// struct FifoMsg
// {
//     Session* session;
//     Msg msg;
// };

int makeFifo(const char* fifo_name);
void sendFifoMsg(int fifofd, const Session* session, const Msg* msg);
std::pair<Session*, Msg*> recvFifoMsg(int fifofd);

#endif // _SCISSORPAPERROCK_FIFO_

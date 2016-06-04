#ifndef _SCISSORPAPERROCK_MSGHANDLER_
#define _SCISSORPAPERROCK_MSGHANDLER_

#include "Message.hpp"
#include <string>

struct Session;

void loginHandler(Session* session, const std::string& username);
void logoutHandler(Session* session);
void shutdownHandler(Session* session);
void infoHandler(Session* session, const std::string& infotype);
void openRoundHandler(Session* session, const std::string& roundname);
void joinRoundHandler(Session* session, const std::string& roundname);
void quitRoundHandler(Session* session);
void castHandler(Session* session, const std::string& gesture);
void roundtimeoutHandler(const std::string& roundname);

void unknownMsgHandler(Session* session);


void msgDispatch();

#endif // _SCISSORPAPERROCK_MSGHANDLER_
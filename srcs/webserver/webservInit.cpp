//
#include "webserv.hpp"
#include <sys/event.h>

using namespace webserv;

#define MAX_PORTS 65535 // maximum ports on this

static int findPWD( char** env ) {
    std::string pwd = "PWD=";
    int i = 0, found;

    for ( ; env[i]; i++ ) {
        found = 1;
        for ( int j = 0; j < 4 && env[i][j]; j++ ) {
            if ( env[i][j] != pwd[j] )
                found = 0;
        }
        if ( found )
            break;
    }
    if ( !found )
        return -1;
    return i;
}

static std::string setFileLocation( char** env ) {
    int pwd = findPWD( env );
    if ( pwd == ERROR )
        return "PWDNOTFOUND";
    std::string current( env[pwd] );
    int pos = current.rfind( "webserv" );
    std::string root = current.substr( 4, pos + 3 );
    return root;
}

static void initServerData( serverData& serverData, httpData& httpData ) {
    int port = httpData.port;
    std::pair<int, std::string> pair;

    if ( serverData.defaultServer.find( port ) == serverData.defaultServer.end() )
        serverData.defaultServer[port] = httpData;
    for ( size_t i = 0; i < httpData.serverName.size(); i++ ) {
        pair = std::make_pair( port, httpData.serverName[i] );
        if ( serverData.hostServername.find( pair ) == serverData.hostServername.end() )
            serverData.hostServername[std::make_pair( port, httpData.serverName[i] )] = httpData;
    }
}

int webserv::init_servers( serverData& serverData, std::string filename, char** env ) {
    std::string root = setFileLocation( env );
    std::string configFile = root;
    configFile.append( "/var/sites_enabled/" );
    configFile.append( filename );
    configParser object( configFile );
    std::vector<httpData> http_vec;
    socketData socket_vec;

    //serverData.closeConnection = false;

    int server = 0, ret = NEOF;

    serverData.buflen = 1024;
    serverData.buf = new char[serverData.buflen + 1];
    serverData.buf[serverData.buflen] = '\0';
    if ( object.checkErrorCode() == ERROR )
        return ERROR;
    do {
        http_vec.push_back( httpData( root ) );
        ret = object.parseIntoPieces( socket_vec, http_vec[server] );
        if ( object.checkErrorCode() == ERROR )
            return ERROR;
        initServerData( serverData, http_vec[server] );
        http_vec.back().env = env;
        server++;
    } while ( ret == NEOF );

    serverData.kqData.workerConnections = socket_vec.workerConnections;
    serverData.kqData.kq = kqueue();
    if ( serverData.kqData.kq == ERROR ) {
        std::cerr << "  kqueue() failed" << std::endl;
        return ERROR;
    }
    struct kevent in_events[MAX_PORTS];
    listeningSocket* socket_tmp;
    int i = 0;
    for ( size_t serv = 0; serv < socket_vec.ports.size(); serv++ ) {
        socket_tmp = new listeningSocket( socket_vec, socket_vec.ports[serv] );
        serverData.serverMap[socket_tmp->get_sock()] = std::make_pair( socket_tmp, serverData.defaultServer[socket_vec.ports[serv]] );
        EV_SET( & in_events[i++], socket_tmp->get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
    }
    return kevent( serverData.kqData.kq, in_events, socket_vec.ports.size(), NULL, 0, NULL );  //  register all listening sockets at once
}

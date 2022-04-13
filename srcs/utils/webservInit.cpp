//
// Created by Kester kas De rooij on 4/6/22.
//

#include "webservInit.hpp"
#include "../config/configParser.hpp"
#include <sys/event.h>

static int findPWD( char **env ) {
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

static std::string setFileLocation( char **env ) {
    int pwd = findPWD( env );
    if ( pwd == ERROR )
        return "PWDNOTFOUND";
    std::string current( env[pwd] );
    int pos = current.find( "webserv" );
    std::string root = current.substr( 4, pos + 3 );
    return root;
}

static void initServerData( webserv::serverData &serverData, webserv::httpData& httpData ){
    int port = httpData.port;
    std::pair<int, std::string> pair;

    if ( serverData.default_server.find(port) == serverData.default_server.end() )
        serverData.default_server[port] = httpData;
    for (int i = 0; i < httpData.server_name.size(); i++) {
        pair = std::make_pair( port, httpData.server_name[i] );
        if ( serverData.host_servername.find( pair ) == serverData.host_servername.end() )
            serverData.host_servername[std::make_pair( port, httpData.server_name[i] )] = httpData;
    }
}

int webserv::init_servers( webserv::serverData &serverData, std::string filename, char **env ) {
    std::string root = setFileLocation( env );
    std::string configFile = root;
    configFile.append( "/var/sites_enabled/" );
    configFile.append( filename );
    webserv::configParser object( configFile );
    std::vector<webserv::httpData> http_vec;
    webserv::socketData socket_vec;

    int server = 0, ret = NEOF;

    serverData.buflen = 1024;
    serverData.buf = new char[serverData.buflen + 1];
    serverData.buf[serverData.buflen] = '\0';
    if ( object.checkErrorCode() == ERROR )
        return ERROR;
    do {
        http_vec.push_back( webserv::httpData( root ) );
        ret = object.parseIntoPieces( socket_vec, http_vec[server] );
        if ( object.checkErrorCode() == ERROR ){
//            for ( ; server >= 0; server-- )
//                delete http_vec[server];
            return ERROR;
        }
        initServerData( serverData, http_vec[server] );
        http_vec.back().env = env;
        server++;
    } while ( ret == NEOF );

    serverData.kqData.worker_connections = socket_vec.worker_connections;
    serverData.kqData.kq = kqueue();
    if ( serverData.kqData.kq == ERROR ) {
        std::cerr << "  kqueue() failed" << std::endl;
        return ERROR;
    }
    struct kevent in_events[socket_vec.ports.size()];
    webserv::listeningSocket *socket_tmp;
    int i = 0;
    for ( int serv = 0; serv < socket_vec.ports.size(); serv++ ) {
        socket_tmp = new webserv::listeningSocket( socket_vec, socket_vec.ports[serv] );
        serverData.serverMap[socket_tmp->get_sock()] = std::make_pair( socket_tmp, serverData.default_server[socket_vec.ports[serv]] );
        EV_SET( &in_events[i++], socket_tmp->get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
    }
    return kevent( serverData.kqData.kq, in_events, socket_vec.ports.size(), NULL, 0, NULL );  //  register all listening sockets at once
}

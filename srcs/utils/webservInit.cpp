//
// Created by Kester kas De rooij on 4/6/22.
//

#include "webservInit.hpp"
#include "../config/configParser.hpp"
#include <sys/event.h>

int webserv::findPWD( char **env ) {
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

std::string webserv::setFileLocation( char **env ) {
    int pwd = findPWD( env );
    if ( pwd == ERROR )
        return "PWDNOTFOUND";
    std::string current( env[pwd] );
    int pos = current.find( "webserv" );
    std::string root = current.substr( 4, pos + 3 );
    return root;
}

int webserv::init_servers( webserv::serverData &serverData, std::string filename, char **env ) {
    std::string root = webserv::setFileLocation( env );
    std::string configFile = root;
    configFile.append( "/var/sites_enabled/" );
    configFile.append( filename );
    webserv::configParser object( configFile );
    std::vector<webserv::httpData *> http_vec;
    std::vector<webserv::socketData *> socket_vec;

    int server = 0, ret = NEOF, sockets = 0;

    serverData.buflen = 1024;
    serverData.buf = new char[serverData.buflen + 1];
    serverData.buf[serverData.buflen] = '\0';
    if ( object.checkErrorCode() == ERROR )
        return ERROR;
    do {
        http_vec.push_back( new webserv::httpData( root ));
        socket_vec.push_back( new webserv::socketData());
        ret = object.parseIntoPieces( socket_vec[server], http_vec[server] );
        if ( object.checkErrorCode() == ERROR ){
            for ( ; server >= 0; server-- ){
                delete socket_vec[server];
                delete http_vec[server];
            }
            return ERROR;
        }
        http_vec.back()->env = env;
        sockets += socket_vec.back()->ports.size();
        server++;
    } while ( ret == NEOF );

    serverData.kqData.worker_connections = socket_vec[0]->worker_connections;
    serverData.kqData.kq = kqueue();
    if ( serverData.kqData.kq == ERROR ) {
        std::cerr << "  kqueue() failed" << std::endl;
        return ERROR;
    }

    struct kevent in_events[sockets];
    webserv::listeningSocket *socket_tmp;
    int i = 0;
    for ( int serv = 0; serv < socket_vec.size(); serv++ ) {
        for ( int sock = 0; sock < socket_vec[serv]->ports.size(); sock++ ) {
            socket_tmp = new webserv::listeningSocket( *socket_vec[serv], socket_vec[serv]->ports[sock] );
            serverData.serverMap[socket_tmp->get_sock()] = std::make_pair( socket_tmp, http_vec[serv] );
            EV_SET( &in_events[i++], socket_tmp->get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
        }
    }
    return kevent( serverData.kqData.kq, in_events, sockets, NULL, 0,
                   NULL );  //  register all listening sockets at once
}

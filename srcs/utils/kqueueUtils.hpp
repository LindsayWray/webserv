//
// Created by Kester kas De rooij on 3/23/22.
//

#ifndef WEBSERV_KQUEUEUTILS_HPP
#define WEBSERV_KQUEUEUTILS_HPP

#include "../config/configParser.hpp"
#include "../utils/stringUtils.hpp"
#include "../server/serverInstance.hpp"

#define SERVER_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData*>>

int init_servers( SERVER_MAP& serverMap, char* filename, char** env, webserv::kqConData& kqData ){
    std::string root = webserv::setFileLocation( env );
    std::string configFile = root;
    configFile.append("/var/sites_enabled/");
    configFile.append(filename);
    webserv::configParser object(configFile );
    std::vector<webserv::httpData*> http_vec;
    std::vector<webserv::socketData*> socket_vec;

    int server = 0, ret = NEOF, sockets = 0;

    do {
        http_vec.push_back( new webserv::httpData(root) );
        socket_vec.push_back( new webserv::socketData() );
        ret = object.parseIntoPieces( socket_vec[server], http_vec[server] );
        if ( ret == ERROR )
            return ERROR;
        if ( server == 0 )
            kqData.worker_connections = socket_vec[0]->worker_connections;
        sockets += socket_vec.back()->ports.size();
    } while ( ret == NEOF );

    kqData.kq = kqueue();
    if ( kqData.kq == ERROR ) {
        std::cerr << "  kqueue() failed" << std::endl;
        return ERROR;
    }

    struct kevent in_events[sockets];

    webserv::listeningSocket* socket_tmp;
    int i =0;
    for ( int serv = 0; serv < socket_vec.size(); serv++ ){
        for ( int sock = 0; sock < socket_vec[serv]->ports.size(); sock++ ) {
            socket_tmp = new webserv::listeningSocket( *socket_vec[serv], socket_vec[serv]->ports[sock] );
            serverMap[socket_tmp->get_sock()] = std::make_pair(socket_tmp, http_vec[serv]);
            EV_SET(&in_events[i++], socket_tmp->get_sock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
        }
    }
    return kevent( kqData.kq, in_events, sockets, NULL, 0, NULL);
}

#endif //WEBSERV_KQUEUEUTILS_HPP

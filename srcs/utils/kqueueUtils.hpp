//
// Created by Kester kas De rooij on 3/23/22.
//

#ifndef WEBSERV_KQUEUEUTILS_HPP
#define WEBSERV_KQUEUEUTILS_HPP

#include "../config/configParser.hpp"
#include "../utils/stringUtils.hpp"
#include "../http/HTTPResponseMessage.hpp"
#include <sys/event.h>
#include "webserv.hpp"

#define MAX_EVENTS 1024

int init_servers( SERVER_MAP& serverMap, std::string filename, char** env, webserv::kqConData& kqData ){
    std::string root = webserv::setFileLocation( env );
    std::string configFile = root;
    configFile.append("/var/sites_enabled/");
    configFile.append(filename);
    webserv::configParser object( configFile );
    std::vector<webserv::httpData*> http_vec;
    std::vector<webserv::socketData*> socket_vec;

    int server = 0, ret = NEOF, sockets = 0;

    /*
     * this next block parses the config file by server{ } block
     * all the ports are saved per server in socketData
     * this might be obsolete in the current setup but works for now
     * all the server specific data is saved in httpData
     *
     * kqData has:
     * kq
     */
    do {
        http_vec.push_back( new webserv::httpData(root) );
        socket_vec.push_back( new webserv::socketData() );
        ret = object.parseIntoPieces( socket_vec[server], http_vec[server] );
        if ( ret == ERROR )
            return ERROR;
        http_vec.back()->env = env;
        sockets += socket_vec.back()->ports.size();
		server++;
    } while ( ret == NEOF );

	kqData.worker_connections = socket_vec[0]->worker_connections;
    kqData.kq = kqueue();
    if ( kqData.kq == ERROR ) {
        std::cerr << "  kqueue() failed" << std::endl;
        return ERROR;
    }


    /*
     * serverMap consists of:
     * first type : fd of the socket we are listening to;
     * second type: pair of listeningSocket and httpData connected to that socket
     *
     * in the next block are all sockets initialized and put in the serverMap
     * also all the listening kevent elements are set with EV_SET
     */
    struct kevent in_events[sockets];
    webserv::listeningSocket* socket_tmp;
    int i = 0;
    for ( int serv = 0; serv < socket_vec.size(); serv++ ){
        for ( int sock = 0; sock < socket_vec[serv]->ports.size(); sock++ ) {
            socket_tmp = new webserv::listeningSocket( *socket_vec[serv], socket_vec[serv]->ports[sock] );
            serverMap[socket_tmp->get_sock()] = std::make_pair(socket_tmp, http_vec[serv]);
            EV_SET(&in_events[i++], socket_tmp->get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        }
    }
    return kevent( kqData.kq, in_events, sockets, NULL, 0, NULL);  //  register all listening sockets at once
}

#endif //WEBSERV_KQUEUEUTILS_HPP

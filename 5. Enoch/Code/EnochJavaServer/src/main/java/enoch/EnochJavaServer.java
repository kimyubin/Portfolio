package enoch;

import enoch.netty.NettyServer;

public class EnochJavaServer {

    public static void main(String[] args) throws Exception {
        System.out.println("Enoch java Server");



        new NettyServer().start();
    }

}

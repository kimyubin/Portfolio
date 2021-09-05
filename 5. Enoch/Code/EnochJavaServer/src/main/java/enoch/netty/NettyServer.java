package enoch.netty;

import enoch.netty.boot.ServerBoot;
import enoch.netty.packet.PacketInitializer;

public class NettyServer {
    private ServerBoot serverBoot;

    public void start() throws InterruptedException {
        serverBoot.serverStart();
    }

    public NettyServer() throws Exception {
        Init();
    }

    private void Init() throws Exception {
        serverBoot = new ServerBoot();

        {
            //싱글톤 인스턴스 불러와서 최초 초기화(패킷 등록과정이라 초기화를 해주는게 좋음)
            PacketInitializer.getInstance();
        }
    }
}

package enoch.netty.boot;

import enoch.netty.handler.ChannelInit;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import java.net.InetSocketAddress;

public class ServerBoot {
    private ChannelInit channelInit = new ChannelInit();

    ServerBootstrap bootstrap;

    EventLoopGroup listenGroup;
    EventLoopGroup workerGroup;

    public ServerBoot() throws Exception {
        init();
    }


    public void serverStart() throws InterruptedException {
        ChannelFuture f = bootstrap.bind().sync();
        f.channel().closeFuture().sync();
    }


    public void shutdown() throws InterruptedException {
        listenGroup.shutdownGracefully().sync(); // Group이 종료될 때 모든 리소스 해제
        workerGroup.shutdownGracefully().sync(); // Group이 종료될 때 모든 리소스 해제
    }


    public void init() throws Exception {
        //TODO thread count 옵션으로?
        listenGroup = new NioEventLoopGroup(2);
        workerGroup = new NioEventLoopGroup(8);

        bootstrap = new ServerBootstrap();

        //TODO 필요 옵션 추가
        bootstrap.group(listenGroup, workerGroup)
                .channel(NioServerSocketChannel.class)
                .localAddress(new InetSocketAddress(8080)) //TODO 임시 8080 포트 고정 -> 세팅추가필요
                .childHandler(channelInit)
                .childOption(ChannelOption.TCP_NODELAY, true)
        ;
        ChannelFuture f = bootstrap.bind().sync();
        f.channel().closeFuture().sync();
    }
}

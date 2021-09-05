package enoch.netty.handler;

import enoch.netty.handler.example.EchoServerHandler;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInitializer;

public class ChannelInit<SocketChannel> extends ChannelInitializer {

    private EchoServerHandler echoServerHandler = new EchoServerHandler();
    private InboundHandler inboundHandler = new InboundHandler();
    private Decoder decoder = new Decoder();

    @Override
    protected void initChannel(Channel channel) throws Exception {
        channel.pipeline().addLast(decoder).addLast(inboundHandler);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        super.exceptionCaught(ctx, cause);
    }

    @Override
    public void handlerAdded(ChannelHandlerContext ctx) throws Exception {
        super.handlerAdded(ctx);
    }

    @Override
    public void handlerRemoved(ChannelHandlerContext ctx) throws Exception {
        super.handlerRemoved(ctx);
    }
}

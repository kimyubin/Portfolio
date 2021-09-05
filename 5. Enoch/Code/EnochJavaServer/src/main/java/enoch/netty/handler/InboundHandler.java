package enoch.netty.handler;

import com.enoch.proto.PMessageProto.PPacket;
import com.google.protobuf.ByteString;
import enoch.netty.packet.PacketInitializer;
import enoch.netty.packet.PacketMethod;
import enoch.netty.packet.message.Packet;
import enoch.netty.packet.message.ReqMessageType;
import enoch.netty.session.UserSession;
import enoch.netty.session.UserSessionManager;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.PooledByteBufAllocator;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;

public class InboundHandler extends ChannelInboundHandlerAdapter {

    UserSessionManager userSessionManager = UserSessionManager.getInstance();

    public Packet dispatch(ChannelHandlerContext ctx, Object msg) throws Exception {
        PPacket packet = (PPacket) msg;
        Channel channel = ctx.channel();


        return packetProcess(channel, packet);
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        userSessionManager.activeSession(ctx.channel());
        System.out.println("active test");//테스트용
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        //TODO session Inactive 추가 필요
        System.out.println("inactive test");//테스트용
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        System.out.println("channel read test");//테스트용

        Packet packet = (Packet) dispatch(ctx, msg);

        // TODO 패킷 처리 부 분리 필요
        if(packet != null) {
            ByteBuf sendBuf = PooledByteBufAllocator.DEFAULT.directBuffer();
//            ByteBuf sendBuf = Unpooled.directBuffer();
            byte[] packetBytes = packet.toProto().toByteArray();
            sendBuf.writeInt(packetBytes.length);
            sendBuf.writeBytes(packetBytes);
            ctx.channel().writeAndFlush(sendBuf);
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        System.out.println("channel read complete test");//테스트용
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        super.exceptionCaught(ctx, cause);
    }

    public Packet packetProcess(Channel channel, PPacket pPacket) throws Exception {
        ReqMessageType messageType = ReqMessageType.fromTypeNum(pPacket.getType());
        ByteString byteString = pPacket.getBody();
        String sessionKey = pPacket.getSessionKey();
        PacketInitializer packetInitializer = PacketInitializer.getInstance();
        PacketMethod packetMethod = null;

        UserSession session = userSessionManager.getActiveSession(sessionKey);


        switch (messageType.getAuthTypeEnum()) {
            case NONE: //모두 허용(주로 테스트용, 필요시 서버간 통신?)
                packetMethod = packetInitializer.getMethod(messageType);
                break;
            case AUTH: // 인증된 세션만 허용
                //TODO 인증된 세션 체크 추가
                break;
            case NONE_AUTH: // 인증되지 않은 세션만 허용(주로 인증용 패킷처리용)
                //TODO 인증되지 않은 세션 체크 추가
                break;
            default:
                break;
        }

        if(packetMethod == null) {
            //TODO exception 처리
            return null;
        }

        return packetMethod.methodRun(session, byteString);
    }
}

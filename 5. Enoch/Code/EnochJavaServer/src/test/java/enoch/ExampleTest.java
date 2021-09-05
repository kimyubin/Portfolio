package enoch;

import com.enoch.proto.PMessageProto.PPacket;
import com.enoch.proto.test.PTestProto.PEcho;
import enoch.netty.handler.Decoder;
import enoch.netty.handler.InboundHandler;
import enoch.netty.packet.message.Packet;
import enoch.netty.packet.message.ReqMessageType;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.PooledByteBufAllocator;
import io.netty.channel.embedded.EmbeddedChannel;
import org.junit.jupiter.api.Test;

import java.io.IOException;

import static org.junit.jupiter.api.Assertions.assertEquals;


class ExampleTest {

    InboundHandler inboundHandler = new InboundHandler();
    Decoder decoder = new Decoder();

    @Test
    public void ExampleTest() throws IOException {
        EmbeddedChannel channel = new EmbeddedChannel();

        channel.pipeline().addLast(decoder).addLast(inboundHandler);

        String echoMsg = "echo test";

        PEcho echo = PEcho.newBuilder().setEcho(echoMsg).build();

        Packet packet = new Packet(ReqMessageType.REQ_ECHO_TEST, echo);

        PPacket pPacket = PPacket.newBuilder().setType(ReqMessageType.REQ_ECHO_TEST.getTypeNum())
                .setBody(echo.toByteString()).build();
//                .setBodySize(message.toByteArray().length)
//                .setMessage(message).build();

        ByteBuf buf = PooledByteBufAllocator.DEFAULT.buffer();
        buf.writeInt(pPacket.toByteArray().length);
        buf.writeBytes(pPacket.toByteArray());


        channel.writeInbound(buf);

        //TODO TC에서 바로 처리할 수 있게 base 클래스 작성 필요
        Object output = channel.readOutbound();
        ByteBuf outputBuf = (ByteBuf)output;
        int size = outputBuf.readInt();
        byte[] outputAry = new byte[size];
        outputBuf.readBytes(outputAry);
        PPacket outMsg = PPacket.getDefaultInstance().getParserForType().parseFrom(outputAry);
        String outEchoMsg = PEcho.parseFrom(outMsg.getBody()).getEcho();

        assertEquals(echoMsg, outEchoMsg);
    }
}
package enoch.netty.handler;

import com.enoch.proto.PMessageProto.PPacket;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToMessageDecoder;

import java.util.List;

public class Decoder extends MessageToMessageDecoder {

    private final static int HEADER_SIZE = Integer.BYTES;


    @Override
    protected void decode(ChannelHandlerContext channelHandlerContext, Object o, List list) throws Exception {
        ByteBuf buf = (ByteBuf)o;

        if(buf.readableBytes() < HEADER_SIZE) {
            return;
        }
        int readSize = buf.readInt();

        if(readSize < 0) {
            //TODO exception 처리
            return;
        }

        if(buf.readableBytes() < readSize) {
            return;
        }

        byte[] array;
        if(buf.hasArray()) {
            array = buf.array();
        } else {
            array = new byte[readSize];
            buf.getBytes(buf.readerIndex(), array,0, readSize);
        }

        list.add(PPacket.getDefaultInstance().getParserForType().parseFrom(array));
    }
}

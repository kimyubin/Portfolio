package enoch.netty.packet.message;

import com.enoch.proto.PMessageProto.PPacket;
import com.google.protobuf.AbstractMessage;
import com.google.protobuf.ByteString;

import java.io.IOException;
public class Packet {

    IMessageType type;
    private ByteString body;

    public ByteString getBody() {
        return body;
    }

    public void setBody(ByteString body) {
        this.body = body;
    }

    public Packet(IMessageType type) {
        this.type = type;
        this.body = null;
    }

    public Packet(IMessageType type, ByteString body) throws IOException {
        this.type = type;
        this.body = body;
    }

    public Packet(IMessageType type, AbstractMessage body) throws IOException {
        this.type = type;
        this.body = body.toByteString();
    }

    public PPacket toProto() throws IOException {
        PPacket.Builder builer = PPacket.newBuilder()
                .setType(type.getTypeNum());

        if(body != null) {
            builer.setBody(body);
        }

        return builer.build();
    }
}

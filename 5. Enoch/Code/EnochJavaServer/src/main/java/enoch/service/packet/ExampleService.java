package enoch.service.packet;

import com.enoch.proto.test.PTestProto.PEcho;
import enoch.netty.packet.PacketAnno;
import enoch.netty.packet.message.Packet;
import enoch.netty.packet.message.ReqMessageType;
import enoch.netty.packet.message.ResMessageType;
import enoch.netty.session.UserSession;

import java.io.IOException;

public class ExampleService extends AbsPacketService {

    @PacketAnno(Header = ReqMessageType.REQ_ECHO_TEST)
    public Packet echo(UserSession session, PEcho msg) throws IOException {
        PEcho echo = PEcho.newBuilder()
                .setEcho(msg.getEcho())
                .build();

        return new Packet(ResMessageType.RES_ECHO, echo);
    }
}

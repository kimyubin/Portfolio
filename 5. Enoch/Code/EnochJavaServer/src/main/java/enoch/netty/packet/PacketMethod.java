package enoch.netty.packet;

import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Parser;
import enoch.netty.packet.message.Packet;
import enoch.netty.session.UserSession;
import enoch.service.packet.AbsPacketService;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.Arrays;
import java.util.List;

public class PacketMethod {
    private AbsPacketService packetService;
    private Method method;
    private Class<?> msgType;

    public PacketMethod(AbsPacketService packetService, Method method) throws Exception {
        this.packetService = packetService;
        this.method = method;

        List<Parameter> parameterList = Arrays.asList(method.getParameters());
        if(!parameterList.isEmpty()) {
            if(!parameterList.get(0).getType().equals(UserSession.class)) {
                throw new Exception();
            }

            if(parameterList.get(1) != null) {
                //TODO instanceof AbstractMessage 체크 필요? -> 런타임에서 초기화 과정에서 오류 체크 위해
                msgType = (Class<?>) parameterList.get(1).getType();
            }
        } else {
            //TODO 별개 익셉션 추가 필요 session이 없는 패킷메소드
            throw new Exception();
        }
    }

    public AbsPacketService getPacketService() {
        return packetService;
    }

    public void setPacketService(AbsPacketService packetService) {
        this.packetService = packetService;
    }

    public Method getMethod() {
        return method;
    }

    public void setMethod(Method method) {
        this.method = method;
    }

    public Class<?> getMsgType() {
        return msgType;
    }

    public void setMsgType(Class<?> msgType) {
        this.msgType = msgType;
    }

    public Packet methodRun(UserSession session, ByteString msgString) throws InvocationTargetException, IllegalAccessException, NoSuchMethodException, InvalidProtocolBufferException {

        Parser<?> parser = (Parser<?>) msgType.getMethod("parser").invoke(null);
        return (Packet) method.invoke(packetService, session, parser.parseFrom(msgString));
    }
}

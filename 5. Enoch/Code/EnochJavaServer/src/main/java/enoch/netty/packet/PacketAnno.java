package enoch.netty.packet;

import enoch.netty.packet.message.ReqMessageType;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Packet 메소드를 읽어오기 위한 애노테이션
 */
@Target({ElementType.ANNOTATION_TYPE, ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
public @interface PacketAnno {

    ReqMessageType Header();
}

package enoch.netty.packet;

public enum AuthTypeEnum {
    NONE, // 모두 허용되는 타입
    AUTH, // 인증된 세션에서만 허용되는 타입
    NONE_AUTH // 인증되지 않은 세션에서 허용되는 타입
}

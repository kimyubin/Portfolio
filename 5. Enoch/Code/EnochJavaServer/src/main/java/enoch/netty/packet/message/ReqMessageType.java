package enoch.netty.packet.message;

import enoch.netty.packet.AuthTypeEnum;

import java.util.Collections;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public enum ReqMessageType implements IMessageType{
    NONE(0,AuthTypeEnum.NONE),//미샤용 번호

    //1~100 테스트용 타입
    REQ_ECHO_TEST(1,AuthTypeEnum.NONE),

    //101~199 인증 등 시스템 타입
    AUTH(100, AuthTypeEnum.NONE_AUTH),


    ;

    private final int typeNum;
    private final AuthTypeEnum authTypeEnum; //인증 정보
    ReqMessageType(Integer typeNum) {
        this.typeNum = typeNum;
        authTypeEnum = AuthTypeEnum.NONE;
    }

    ReqMessageType(Integer typeNum, AuthTypeEnum authTypeEnum) {
        this.typeNum = typeNum;
        this.authTypeEnum = authTypeEnum;
    }

    @Override
    public int getTypeNum() {
        return typeNum;
    }

    public AuthTypeEnum getAuthTypeEnum() {
        return authTypeEnum;
    }

    private static final Map<Integer ,String> typeMap = Collections.unmodifiableMap(Stream.of(values()).
            collect(Collectors.toUnmodifiableMap(ReqMessageType::getTypeNum, ReqMessageType::name)));

    public static ReqMessageType fromTypeNum(int typeNum) {
        return ReqMessageType.valueOf(typeMap.get(typeNum));
    }
}

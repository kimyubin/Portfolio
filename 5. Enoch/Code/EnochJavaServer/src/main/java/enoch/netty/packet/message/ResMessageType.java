package enoch.netty.packet.message;

public enum ResMessageType implements IMessageType{
    NONE(0),
    RES_ECHO(1);


    private int typeNum;
    ResMessageType(int typeNum) {
        this.typeNum = typeNum;
    };

    @Override
    public int getTypeNum() {
        return typeNum;
    }
}

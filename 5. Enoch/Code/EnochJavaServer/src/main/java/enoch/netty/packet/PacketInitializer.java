package enoch.netty.packet;

import enoch.netty.packet.message.IMessageType;
import enoch.service.packet.AbsPacketService;
import enoch.service.packet.ExampleService;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

//싱글톤으로?
public class PacketInitializer {

    private static PacketInitializer instance;
    private  List<AbsPacketService> packetManageList = new ArrayList<>();
    private  Map<IMessageType, PacketMethod> methodMap = new HashMap<>();

    private PacketInitializer() throws Exception {
        Init();
    }

    /**
     * PacketService 등록(PacetService들은 반드시 여기서 등록해줘야함)
     * @throws InstantiationException
     * @throws IllegalAccessException
     */
    private void manageListInit() throws InstantiationException, IllegalAccessException {
        packetManageList.add(AbsPacketService.getInstance(ExampleService.class));
    }

    public static PacketInitializer getInstance() throws Exception {
        if(instance == null) {
            synchronized (PacketInitializer.class) {
                if(instance == null) {
                    instance = new PacketInitializer();
//                    instance.Init();
                }
            }
        }
        return instance;
    }

    public PacketMethod getMethod(IMessageType msgType) {
        return methodMap.get(msgType);
    }

    private void Init() throws Exception {
        manageListInit();

        for(AbsPacketService obj : packetManageList) {
            for(Method method : obj.getClass().getMethods()) {
                PacketAnno packetAnno = method.getAnnotation(PacketAnno.class);
                if(packetAnno != null) {
                    if(methodMap.get(packetAnno.Header()) != null) {
                        throw new Exception(); //TODO 임시 -> 명확한 익셉션 처리가 필요할거임 중복 header 이슈 체크할 수 있도록
                    }

                    methodMap.put(packetAnno.Header(), new PacketMethod(obj,method));
                }
            }
        }
    }

}

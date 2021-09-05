package enoch.service.packet;

import enoch.service.serviceEnum.ServiceType;
import enoch.service.IBaseService;

import java.util.*;

/**
 * 반드시 싱글톤으로만 사용
 * 추상클래스 싱글톤의 한계로 해당서비스는 같은 패키지에 넣고 packet패키지 내에서만 생성해야함
 * PacketService의 getInstance는 반드시 PacketInitializer에서만 사용 -> 메소드 등록용
 * FIXME 애노테이션으로 처리가 가능할지??
 */
public abstract class AbsPacketService implements IBaseService {

    private static final Map<Class<? extends AbsPacketService>, AbsPacketService> instances
            = new HashMap<Class<? extends AbsPacketService>, AbsPacketService>();

    AbsPacketService() {}

    public static AbsPacketService getInstance(Class<? extends AbsPacketService> cls) throws InstantiationException, IllegalAccessException {
        if(instances.get(cls) == null) {
            synchronized (instances) {
                if(instances.get(cls) == null) {
                    AbsPacketService service = cls.newInstance();
                    instances.put(cls, cls.newInstance());
                }
            }
        }
        return instances.get(cls);
    }

    @Override
    public Set<ServiceType> getServiceTypes() {
        return Collections.unmodifiableSet(new HashSet<>(Arrays.asList(ServiceType.PACKET)));
    }

    @Override
    public Boolean isPacketService() {
        return true;
    }
}

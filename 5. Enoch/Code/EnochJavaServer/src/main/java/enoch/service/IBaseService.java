package enoch.service;

import enoch.service.serviceEnum.ServiceType;

import java.util.Set;

public interface IBaseService {
    Set<ServiceType> getServiceTypes() ;

    default Boolean isPacketService() {
        return  false;
    }
}

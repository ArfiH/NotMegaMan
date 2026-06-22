#include "Physics.h"
#include <iostream>


vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    // TODO: return the overlap rectangle size of the bounding boxes of entity a and b
    float x1 = a->getComponent<CTransform>().pos.x; 
    float y1 = a->getComponent<CTransform>().pos.y; 
    float w1 = a->getComponent<CBoundingBox>().halfSize.x;
    float h1 = a->getComponent<CBoundingBox>().halfSize.y;
    
    float x2 = b->getComponent<CTransform>().pos.x; 
    float y2 = b->getComponent<CTransform>().pos.y; 
    float w2 = a->getComponent<CBoundingBox>().halfSize.x;
    float h2 = a->getComponent<CBoundingBox>().halfSize.y;
    
    float dx = abs(x2 - x1);
    float distSumX = w1 + w2;
    float dy = abs(y2 - y1);
    float distSumY = h1 + h2;
    
    float ox = distSumX - dx;
    float oy = distSumY - dy;
    if (ox > 0 && oy > 0) {    
        std::cerr << "Overlap area: " << ox << ' ' << oy << '\n';
    }
    return vec2(ox, oy);
}

vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    // TODO: return the previous overlap rectangle size of the bounding boxes of entity a and b
    //       previous overlap uses the entity's previous position
    return vec2(0, 0);
}

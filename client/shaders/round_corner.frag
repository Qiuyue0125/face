#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float radius;
    vec2 size;
};
layout(binding = 1) uniform sampler2D source;
void main() {

    // 宽、长 | 实际坐标
    float width = size.x;
    float height = size.y;
    //  最大半径  最小边的一半
    float maxRadius = min(width,height)/2;
    // 实际半径
    float r = min(maxRadius,radius);

    // 当前坐标
    vec2 coord = vec2(qt_TexCoord0.x*width,qt_TexCoord0.y*height);

    // 左上，左下，右下，右上 坐标
    vec2 upperLeft,upperRight,lowerLeft,lowerRight;
    upperLeft = vec2(r,height - r);
    lowerLeft = vec2(r,r);
    lowerRight = vec2(width - r,r);
    upperRight = vec2(width - r,height - r);

    if((coord.x > r && coord.x < width - r) ||
            (coord.y > r && coord.y < height - r)){
        fragColor = texture(source, qt_TexCoord0)* qt_Opacity;
    }
    else{
        if(distance(upperLeft,coord) <r ||
                distance(upperRight,coord)  < r||
                distance(lowerLeft,coord) < r ||
                distance(lowerRight,coord) < r)
            fragColor = texture(source, qt_TexCoord0)* qt_Opacity;
        else
            fragColor =vec4(0)* qt_Opacity;
    }
}


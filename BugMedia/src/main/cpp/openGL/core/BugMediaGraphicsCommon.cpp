//
// Created by Gshine on 2021/2/3.
//

#include "BugMediaGraphicsCommon.h"

//
// 获取图片显示时的宽和高
//
// newWidth:返回的宽度值
// newHeight:返回的高度值
// viewWidth：视图宽度，可见区域的宽度
// viewHeight：视图高度，可见区域的高度
// picWidth：图片宽度
// picHeight：图片高度
void getShowPictureSize(GLint &newWidth, GLint &newHeight, EGLint &viewWidth, EGLint &viewHeight, GLint &picWidth,
                        GLint &picHeight) {

    newWidth = viewWidth;
    newHeight = viewWidth * picHeight / picWidth;

    if (viewWidth > viewHeight) { // 横屏
        if (picWidth > picHeight) { // 横屏横图
            // 让图像宽度匹配(等于)视图宽度后,高度一定小于等于视图高度
            // 计算收缩后的高度:
            // picWidth/picHeight=viewWidth/x
            // x=viewWidth*picHeight/picWidth
            newWidth = viewWidth; // 图像宽度现在变成了视图宽度
            newHeight = viewWidth * picHeight / picWidth;
        } else { // 横屏竖图

            // 图像[新高度]匹配视图高度，宽度一定小于等于视图宽度
            // picWidth/picHeight=x/viewHeight
            // x = viewHeight*picWidth/picHeight
            newHeight = viewHeight;
            newWidth = viewHeight * picWidth / picHeight;
        }


    } else { // 竖屏

        // 图像小就拉伸，大收缩
        // 竖屏竖图与横图操作相同
        //
        // 先让图像宽度匹配(等于)视图,计算收缩后的高度:
        // picWidth/picHeight=viewWidth/x
        // x=viewWidth*picHeight/picWidth
        newWidth = viewWidth; // 图像宽度现在变成了视图宽度
        newHeight = viewWidth * picHeight / picWidth;

        //收缩后如果图像高度还是比视图高度大，则使图像[新高度]匹配视图高度，再收缩宽度
        if (newHeight > viewHeight) {

            // 再次收缩后的图像宽度:
            // newWidth/ newHeight = x/ viewHeight
            // x = viewHeight*newWidth/newHeight
            newWidth = viewHeight * newWidth / newHeight;
        }


    }
}

// 将图形进行缩放并移到视图的中间
void scaleCenter(EGLint &viewWidth, EGLint &viewHeight, EGLint &width, EGLint &height) {
    GLint newWidth;
    GLint newHeight;
    getShowPictureSize(newWidth, newHeight, viewWidth, viewHeight, width, height);
    moveToCenter(viewWidth, viewHeight, newWidth, newHeight);

}

// 将指定宽高的图形移到视图中间
void moveToCenter(EGLint &viewWidth, EGLint &viewHeight, GLint &width, GLint &height) {
    //
    // 设置视角，让图像居中且不变形。以左下角为原点，让图像偏移，注意centerX和centerY并非
    // 图像中心点，只是个相对于左下角的偏移。
    GLint centerX = (viewWidth - width) / 2;
    GLint centerY = (viewHeight - height) / 2;
    glViewport(centerX, centerY, width, height);
}

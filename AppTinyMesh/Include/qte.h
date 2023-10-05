#ifndef __Qte__
#define __Qte__

#include <QtWidgets/qmainwindow.h>
#include "realtime.h"
#include "meshcolor.h"

enum Operation
{
    SPHERE = 0,
    CUBE = 1,
    CONE = 2,
    TORE = 3,
    CAPSULE = 4,
    TRANSLATION = 5,
    ROTATION = 6,
    SCALE = 7,
    UNION = 8,
    INTERSECTION = 9,
    DIFF = 10,
    UNION_SMOOTH = 11,
    INTERSECTION_SMOOTH = 12,
    DIFF_SMOOTH = 13
};

enum ID_Mesh
{
    ID_SPHERE = 0,
    ID_CUBE = 1,
    ID_CONE = 2,
    ID_TORE = 3,
    ID_CAPSULE = 4
};


QT_BEGIN_NAMESPACE
namespace Ui { class Assets; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Assets* uiw;           //!< Interface

    MeshWidget* meshWidget;   //!< Viewer
    MeshColor meshColor;		//!< Mesh.

public:
    MainWindow();
    ~MainWindow();
    void CreateActions();
    void UpdateGeometry();

public slots:
    void editingSceneLeft(const Ray&);
    void editingSceneRight(const Ray&);
    void BoxMeshExample();
    void OperationExample(Operation operation);
    void meshRotation();
    void spawnCube();
    void spawnTore();
    void spawnSphere();
    void spawnCone();
    void spawnCapsule();
    void originalPrimitivesComposition();
    void ResetCamera();
    void UpdateMaterial();
private slots:
    void on_RotateXslider_valueChanged(int value);
    void on_RotateYslider_valueChanged(int value);
    void on_RotateZslider_valueChanged(int value);
};

#endif

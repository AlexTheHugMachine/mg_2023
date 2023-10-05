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
    void spawnCube();
    void spawnTore();
    void originalPrimitivesComposition();
    void ResetCamera();
    void UpdateMaterial();
};

#endif

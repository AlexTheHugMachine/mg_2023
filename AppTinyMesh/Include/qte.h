#ifndef __Qte__
#define __Qte__

#include <QtWidgets/qmainwindow.h>
#include "realtime.h"
#include "meshcolor.h"
#include "signed.h"
#include "node.h"

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

    Signed sdf;
    Node* currentNode;

public slots:
    void editingSceneLeft(const Ray&);
    void editingSceneRight(const Ray&);
    void editingErosion(const Ray&);
    void BoxMeshExample();
    void meshRotation();
    void meshScale();
    void meshTranslation();
    void spawnCube();
    void spawnTore();
    void spawnSphere();
    void spawnCone();
    void spawnCapsule();
    void mixUnion();
    void mixUnionSmooth();
    void mixIntersection();
    void mixIntersectionSmooth();
    void mixDifference();
    void mixDifferenceSmooth();
    void CompositionVisage();
    void ResetCamera();
    void UpdateMaterial();
private slots:
    void on_RotateXslider_valueChanged(int value);
    void on_RotateYslider_valueChanged(int value);
    void on_RotateZslider_valueChanged(int value);
    void on_ScaleXBox_valueChanged(double arg1);
    void on_ScaleYBox_valueChanged(double arg1);
    void on_ScaleZBox_valueChanged(double arg1);
    void on_TranslateXBox_valueChanged(double arg1);
    void on_TranslateYBox_valueChanged(double arg1);
    void on_TranslateZBox_valueChanged(double arg1);
};

#endif

#include "qte.h"
#include "implicits.h"
#include "ui_interface.h"
#include "node.h"
#include "sdf.h"
#include <iostream>

using namespace std;

int RX, RY, RZ;
double scale_factorX, scale_factorY, scale_factorZ, TX, TY, TZ;
ID_Mesh id_m;

MainWindow::MainWindow() : QMainWindow(), uiw(new Ui::Assets)
{
    // Chargement de l'interface
    uiw->setupUi(this);

    // Chargement du GLWidget
    meshWidget = new MeshWidget;
    QGridLayout* GLlayout = new QGridLayout;
    GLlayout->addWidget(meshWidget, 0, 0);
    GLlayout->setContentsMargins(0, 0, 0, 0);
    uiw->widget_GL->setLayout(GLlayout);

    // Creation des connect
    CreateActions();

    meshWidget->SetCamera(Camera(Vector(10, 0, 0), Vector(0.0, 0.0, 0.0)));
}

MainWindow::~MainWindow()
{
    delete meshWidget;
}

void MainWindow::CreateActions()
{
    // Buttons
    connect(uiw->boxMesh, SIGNAL(clicked()), this, SLOT(BoxMeshExample()));
    connect(uiw->sphere, &QPushButton::clicked, [=]() {
        spawnSphere();
        id_m = ID_SPHERE;
    });
    connect(uiw->cube, &QPushButton::clicked, [=]() {
        spawnCube();
        id_m = ID_CUBE;
    });
    connect(uiw->cone, &QPushButton::clicked, [=]() {
        spawnCone();
        id_m = ID_CONE;
    });
    connect(uiw->tore, &QPushButton::clicked, [=]() {
        spawnTore();
        id_m = ID_TORE;
    });
    connect(uiw->capsule, &QPushButton::clicked, [=]() {
        spawnCapsule();
        id_m = ID_CAPSULE;
    });
    connect(uiw->union_, &QPushButton::clicked, [=]() {
        mixUnion();
    });
    connect(uiw->intersection, &QPushButton::clicked, [=]() {
        mixIntersection();
    });
    connect(uiw->difference, &QPushButton::clicked, [=]() {
        mixDifference();
    });
    connect(uiw->union_lisse, &QPushButton::clicked, [=]() {
        mixUnionSmooth();
    });
    connect(uiw->intersection_lisse, &QPushButton::clicked, [=]() {
        mixIntersectionSmooth();
    });
    connect(uiw->difference_lisse, &QPushButton::clicked, [=]() {
        mixDifferenceSmooth();
    });

    connect(uiw->composition, SIGNAL(clicked()), this, SLOT(CompositionVisage()));
    connect(uiw->resetcameraButton, SIGNAL(clicked()), this, SLOT(ResetCamera()));
    connect(uiw->wireframe, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_1, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_2, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->RotateXslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateYslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateZslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->ScaleXBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->ScaleYBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->ScaleZBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->TranslateXBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));
    connect(uiw->TranslateYBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));
    connect(uiw->TranslateZBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));

    // Widget edition
    connect(meshWidget, SIGNAL(_signalEditSceneLeft(const Ray&)), this, SLOT(editingSceneLeft(const Ray&)));
    connect(meshWidget, SIGNAL(_signalEditSceneRight(const Ray&)), this, SLOT(editingSceneRight(const Ray&)));
}

void MainWindow::editingSceneLeft(const Ray&)
{
}

void MainWindow::editingSceneRight(const Ray&)
{
}

void MainWindow::BoxMeshExample()
{
    Mesh boxMesh = Mesh(Box(1.0));

    std::vector<Color> cols;
    cols.resize(boxMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(double(i) / 6.0, fmod(double(i) * 39.478378, 1.0), 0.0);

    meshColor = MeshColor(boxMesh, cols, boxMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCube()
{
    Mesh implicitMesh;
    Cube cube = Cube(Vector(0, 0, 0), Vector(3, 3, 3));
    SDF sdf = SDF(&cube);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnTore()
{
    Mesh implicitMesh;
    Tore tore = Tore(1, 3);
    SDF sdf = SDF(&tore);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnSphere()
{
    Mesh implicitMesh;
    Sphere sphere = Sphere(Vector(0, 0, 0), 2);
    SDF sdf = SDF(&sphere);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCone()
{
    Mesh implicitMesh;
    Cone cone = Cone(0.2, 0.6, 5);
    SDF sdf = SDF(&cone);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCapsule()
{
    Mesh implicitMesh;
    Capsule capsule = Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
    SDF sdf = SDF(&capsule);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::on_RotateXslider_valueChanged(int value)
{
    RX = value;
}

void MainWindow::on_RotateYslider_valueChanged(int value)
{
    RY = value;
}

void MainWindow::on_RotateZslider_valueChanged(int value)
{
    RZ = value;
}

void MainWindow::on_ScaleXBox_valueChanged(double arg1)
{
    scale_factorX = arg1;
}

void MainWindow::on_ScaleYBox_valueChanged(double arg1)
{
    scale_factorY = arg1;
}

void MainWindow::on_ScaleZBox_valueChanged(double arg1)
{
    scale_factorZ = arg1;
}

void MainWindow::on_TranslateXBox_valueChanged(double arg1)
{
    TX = arg1;
}

void MainWindow::on_TranslateYBox_valueChanged(double arg1)
{
    TY = arg1;
}

void MainWindow::on_TranslateZBox_valueChanged(double arg1)
{
    TZ = arg1;
}

void MainWindow::meshRotation()
{
    Mesh implicitMesh;
    Rotation * rotationPrimitives;
    SDF sdf;
    Sphere * sphere_rotationX;
    Cube * cube_rotationX;
    Cone * cone_rotationX;
    Tore * tore_rotationX;
    Capsule * capsule_rotationX;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_rotationX = new Sphere(Vector(0, 0, 0), 2);
        rotationPrimitives = new Rotation(sphere_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CUBE:
        cube_rotationX = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        rotationPrimitives = new Rotation(cube_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CONE:
        cone_rotationX = new Cone(0.2, 0.6, 5);
        rotationPrimitives = new Rotation(cone_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_TORE:
        tore_rotationX = new Tore(1, 3);
        rotationPrimitives = new Rotation(tore_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CAPSULE:
        capsule_rotationX = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        rotationPrimitives = new Rotation(capsule_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::meshScale()
{
    Mesh implicitMesh;
    Scale * scalePrimitives;
    SDF sdf;
    Sphere * sphere_scale;
    Cube * cube_scale;
    Cone * cone_scale;
    Tore * tore_scale;
    Capsule * capsule_scale;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_scale = new Sphere(Vector(0, 0, 0), 2);
        scalePrimitives = new Scale(sphere_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = SDF(scalePrimitives);
        break;
    case ID_CUBE:
        cube_scale = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        scalePrimitives = new Scale(cube_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = SDF(scalePrimitives);
        break;
    case ID_CONE:
        cone_scale = new Cone(0.2, 0.6, 5);
        scalePrimitives = new Scale(cone_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = SDF(scalePrimitives);
        break;
    case ID_TORE:
        tore_scale = new Tore(1, 3);
        scalePrimitives = new Scale(tore_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = SDF(scalePrimitives);
        break;
    case ID_CAPSULE:
        capsule_scale = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        scalePrimitives = new Scale(capsule_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = SDF(scalePrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::meshTranslation()
{
    Mesh implicitMesh;
    Translation * translatePrimitives;
    SDF sdf;
    Sphere * sphere_translate;
    Cube * cube_translate;
    Cone * cone_translate;
    Tore * tore_translate;
    Capsule * capsule_translate;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_translate = new Sphere(Vector(0, 0, 0), 2);
        translatePrimitives = new Translation(sphere_translate, Vector(TX, TY, TZ));
        sdf = SDF(translatePrimitives);
        break;
    case ID_CUBE:
        cube_translate = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        translatePrimitives = new Translation(cube_translate, Vector(TX, TY, TZ));
        sdf = SDF(translatePrimitives);
        break;
    case ID_CONE:
        cone_translate = new Cone(0.2, 0.6, 5);
        translatePrimitives = new Translation(cone_translate, Vector(TX, TY, TZ));
        sdf = SDF(translatePrimitives);
        break;
    case ID_TORE:
        tore_translate = new Tore(1, 3);
        translatePrimitives = new Translation(tore_translate, Vector(TX, TY, TZ));
        sdf = SDF(translatePrimitives);
        break;
    case ID_CAPSULE:
        capsule_translate = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        translatePrimitives = new Translation(capsule_translate, Vector(TX, TY, TZ));
        sdf = SDF(translatePrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixUnion()
{
    Mesh implicitMesh;
    Union * unionPrimitives;
    SDF sdf;
    Sphere * sphere_Union;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(sphere, sphere_Union);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(cube, sphere_Union);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(cone, sphere_Union);
        sdf = SDF(unionPrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(tore, sphere_Union);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(capsule, sphere_Union);
        sdf = SDF(unionPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixUnionSmooth()
{
    Mesh implicitMesh;
    UnionSmooth * unionPrimitives;
    SDF sdf;
    Sphere * sphere_Union;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new UnionSmooth(sphere, sphere_Union, 4);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new UnionSmooth(cube, sphere_Union, 4);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new UnionSmooth(cone, sphere_Union, 4);
        sdf = SDF(unionPrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new UnionSmooth(tore, sphere_Union, 4);
        sdf = SDF(unionPrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new UnionSmooth(capsule, sphere_Union, 4);
        sdf = SDF(unionPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixIntersection()
{
    Mesh implicitMesh;
    Intersection * intersectionPrimitives;
    SDF sdf;
    Sphere * sphere_Intersection;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(sphere, sphere_Intersection);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(cube, sphere_Intersection);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(cone, sphere_Intersection);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(tore, sphere_Intersection);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(capsule, sphere_Intersection);
        sdf = SDF(intersectionPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixIntersectionSmooth()
{
    Mesh implicitMesh;
    IntersectionSmooth * intersectionPrimitives;
    SDF sdf;
    Sphere * sphere_Intersection;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new IntersectionSmooth(sphere, sphere_Intersection, 4);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new IntersectionSmooth(cube, sphere_Intersection, 4);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new IntersectionSmooth(cone, sphere_Intersection, 4);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new IntersectionSmooth(tore, sphere_Intersection, 4);
        sdf = SDF(intersectionPrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new IntersectionSmooth(capsule, sphere_Intersection, 4);
        sdf = SDF(intersectionPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixDifference()
{
    Mesh implicitMesh;
    Difference * differencePrimitives;
    SDF sdf;
    Sphere * sphere_Difference;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new Difference(sphere, sphere_Difference);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new Difference(cube, sphere_Difference);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new Difference(cone, sphere_Difference);
        sdf = SDF(differencePrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Difference = new Sphere(Vector(1.5, 0, 0), 2);
        differencePrimitives = new Difference(tore, sphere_Difference);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new Difference(capsule, sphere_Difference);
        sdf = SDF(differencePrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixDifferenceSmooth()
{
    Mesh implicitMesh;
    DifferenceSmooth * differencePrimitives;
    SDF sdf;
    Sphere * sphere_Difference;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new DifferenceSmooth(sphere, sphere_Difference, 4);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new DifferenceSmooth(cube, sphere_Difference, 4);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new DifferenceSmooth(cone, sphere_Difference, 4);
        sdf = SDF(differencePrimitives);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Difference = new Sphere(Vector(1.5, 0, 0), 2);
        differencePrimitives = new DifferenceSmooth(tore, sphere_Difference, 4);
        sdf = SDF(differencePrimitives);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new DifferenceSmooth(capsule, sphere_Difference, 4);
        sdf = SDF(differencePrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::CompositionVisage()
{
    Mesh implicitMesh;

    const double smoothFactor = 4;

    Tore * toreBouche = new Tore(1, 3);
    Sphere * sphereBouche = new Sphere(Vector(1, 0, 0), 2);
    IntersectionSmooth * interBouche = new IntersectionSmooth(toreBouche, sphereBouche, smoothFactor);
    Translation * translateBouche = new Translation(interBouche, Vector(0, -1, 0));

    Cone * cou = new Cone(0.2, 0.6, 4.5);
    Sphere * tete = new Sphere(Vector(1, 0, 0), 2);
    Sphere * eye1 = new Sphere(Vector(4, 1.5, 1.5), 2);
    Sphere * eye1b = new Sphere(Vector(15, 4.1, 4.1), 2);
    Scale * scaleEye1 = new Scale(eye1, Vector(0.5, 0.5, 0.5));
    Scale * scaleEye1b = new Scale(eye1b, Vector(0.2, 0.2, 0.2));
    Sphere * eye2 = new Sphere(Vector(4, 1.5, -1.5), 2);
    Sphere * eye2b = new Sphere(Vector(15, 4.1, -4.1), 2);
    Scale * scaleEye2 = new Scale(eye2, Vector(0.5, 0.5, 0.5));
    Scale * scaleEye2b = new Scale(eye2b, Vector(0.2, 0.2, 0.2));

    UnionSmooth * unionCouTete = new UnionSmooth(cou, tete, smoothFactor);
    UnionSmooth * unionVisageEye1 = new UnionSmooth(unionCouTete, scaleEye1, smoothFactor);
    UnionSmooth * unionVisageEye2 = new UnionSmooth(unionVisageEye1, scaleEye2, smoothFactor);
    DifferenceSmooth * diffeye1visage = new DifferenceSmooth(unionVisageEye2, scaleEye1b, smoothFactor);
    DifferenceSmooth * diffeye2visage = new DifferenceSmooth(diffeye1visage, scaleEye2b, smoothFactor);
    DifferenceSmooth * visageComplet = new DifferenceSmooth(diffeye2visage, translateBouche, smoothFactor);
    Rotation * rotationFinale = new Rotation(visageComplet, Vector(-90, 90, 0));

    SDF sdf(rotationFinale);
    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::UpdateGeometry()
{
    meshWidget->ClearAll();
    meshWidget->AddMesh("BoxMesh", meshColor);

    uiw->lineEdit->setText(QString::number(meshColor.Vertexes()));
    uiw->lineEdit_2->setText(QString::number(meshColor.Triangles()));

    UpdateMaterial();
}

void MainWindow::UpdateMaterial()
{
    meshWidget->UseWireframeGlobal(uiw->wireframe->isChecked());

    if (uiw->radioShadingButton_1->isChecked())
        meshWidget->SetMaterialGlobal(MeshMaterial::Normal);
    else
        meshWidget->SetMaterialGlobal(MeshMaterial::Color);
}

void MainWindow::ResetCamera()
{
    meshWidget->SetCamera(Camera(Vector(-10.0), Vector(0.0)));
}

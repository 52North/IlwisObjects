#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "operationnode.h"
#include "conditionNode.h"
#include "rangenode.h"
#include "rangejunctionnode.h"
#include "junctionNode.h"
#include "rangejunctionnode.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "uicontextmodel.h"
#include "operationmodel.h"
#include "operationcatalogmodel.h"
#include "workflowmodel.h"
#include "workflowgraph.h"



using namespace Ilwis;
using namespace Ui;

void WorkflowGraphNodeBehaviour::inNodeInserted(qan::Node& inNode, qan::Edge& edge) noexcept
{
	Q_UNUSED(edge);
	const auto inFlowNode = qobject_cast<WorkflowGraphNode*>(&inNode);
	const auto flowNodeHost = qobject_cast<WorkflowGraphNode*>(getHost());
	if (inFlowNode != nullptr &&
		flowNodeHost != nullptr) {
		//
		QObject::connect(inFlowNode, &WorkflowGraphNode::outputChanged,
			flowNodeHost, &WorkflowGraphNode::inNodeOutputChanged);
	}
	flowNodeHost->inNodeOutputChanged();    // Force a call since with a new edge insertion, actual value might aready be initialized
}

void    WorkflowGraphNodeBehaviour::inNodeRemoved(qan::Node& inNode, qan::Edge& edge) noexcept
{
	Q_UNUSED(inNode); Q_UNUSED(edge);
}

QQmlComponent *ConditionGroup::delegate(QQmlEngine &engine) noexcept {
	static std::unique_ptr<QQmlComponent> delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/ConditionNode.qml");
	return delegate.get();
}

qan::Style *ConditionGroup::style() noexcept {
	static std::unique_ptr<qan::Style> style;
	if (!style) {
		style = std::make_unique<qan::Style>();
	}
	return style.get();
}

void ConditionGroup::setTest(ConditionTestValue *tst) {
	_testNode = tst;
}

void ConditionGroup::addNode(qan::NodeItem *nodeItem) {
	WorkflowGraphNode *wfnode = static_cast<WorkflowGraphNode *>(nodeItem->getNode());
	if (wfnode->getType() == WorkflowGraphNode::Type::Operation){
		auto id = wfnode->nodeId();
		SPWorkFlowNode wfnode = workflow()->workflow()->nodeById(id);
		workflow()->workflow()->removeNode(wfnode->id()); // remove it from the graph is at will be re-added as subnode of the condition
		wfnode->owner(_condition);
		_condition->addSubNode(wfnode,"operations");
	}
}

Ilwis::Ui::ConditionJunction* ConditionGroup::addJunction() {
	ConditionJunction *junction = static_cast<ConditionJunction *>(getGraph()->insertNode<ConditionJunction>(nullptr));
	int midx = getItem()->x() + getItem()->width() / 2;
	int bottomy = getItem()->y() + getItem()->height();
	junction->getItem()->setX(midx);
	junction->getItem()->setY(bottomy + 50);
	junction->getItem()->setResizable(false);
	//junction->getItem()->setDraggable(true);

	auto *edge = getGraph()->insertEdge(this, junction);
	edge->getItem()->setStyle(static_cast<WorkflowGraph *>(getGraph())->junctionEdgeStyle());
	edge->getItem()->getStyle()->setLineColor(uicontext()->lightColor());
	edge->getItem()->getStyle()->setDstShape(qan::EdgeStyle::ArrowShape::None);

	//junction->setWorkflowNode(static_cast<WorkflowGraph *>(getGraph())->workflow()->workflow()->nodeById(nodeId));

	return junction;

}

void ConditionGroup::setConditionNode(SPCondition node) {
	_condition = node;
	if (_condition) {
		setLabel(_condition->name());
	}
}

WorkflowModel *ConditionGroup::workflow() {
	return static_cast<WorkflowGraph *>(getGraph())->workflow();
}

int	ConditionGroup::nodeId() {
	if (_condition) {
		return _condition->id();
	}
	return iUNDEF;
}

void ConditionGroup::updateBoundingBox() {
	if (_condition) {
		auto item = getItem();
		_condition->box(BoundingBox(Pixel(item->x(), item->y()), Pixel(item->x() + item->width(), item->y() + item->height())));
	}
}

void ConditionGroup::addNode2GroupNode(qan::Node *node) {
	auto gitem = static_cast<qan::GroupItem *>(getItem());
	gitem->groupNodeItem(node->getItem(), false);
}

QQmlComponent *RangeGroup::delegate(QQmlEngine &engine) noexcept {
	static std::unique_ptr<QQmlComponent> delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/RangeNode.qml");
	return delegate.get();
}

qan::Style *RangeGroup::style() noexcept {
	static std::unique_ptr<qan::Style> style;
	if (!style) {
		style = std::make_unique<qan::Style>();
	}
	return style.get();
}

void RangeGroup::addNode(qan::NodeItem *nodeItem) {
	WorkflowGraphNode *wfnode = static_cast<WorkflowGraphNode *>(nodeItem->getNode());
	if (wfnode->getType() == WorkflowGraphNode::Type::Operation) {
		auto id = wfnode->nodeId();
		SPWorkFlowNode wfnode = workflow()->workflow()->nodeById(id);
		workflow()->workflow()->removeNode(wfnode->id()); // remove it from the graph is at will be re-added as subnode of the range
		wfnode->owner(_range);
		_range->addSubNode(wfnode, "operations");


	}
}

void RangeGroup::addNode2GroupNode(qan::Node *node) {
	auto gitem = static_cast<qan::GroupItem *>(getItem());
	gitem->groupNodeItem(node->getItem(), false);
}

void RangeGroup::setRangeValue(RangeValue *tst) {
	_rangeValue = tst;
}

void RangeGroup::updateBoundingBox() {
	if (_range) {
		auto item = getItem();
		_range->box(BoundingBox(Pixel(item->x(), item->y()), Pixel(item->x() + item->width(), item->y() + item->height())));
	}
}


Ilwis::Ui::RangeJunction* RangeGroup::addJunction() {
	RangeJunction *junction = static_cast<RangeJunction *>(getGraph()->insertNode<RangeJunction>(nullptr));
	int midx = getItem()->x() + getItem()->width() / 2;
	int bottomy = getItem()->y() + getItem()->height();
	junction->getItem()->setX(midx);
	junction->getItem()->setY(bottomy + 50);
	junction->getItem()->setResizable(false);
	//junction->getItem()->setDraggable(true);

	auto *edge = getGraph()->insertEdge(this, junction);
	edge->getItem()->setStyle(static_cast<WorkflowGraph *>(getGraph())->junctionEdgeStyle());
	edge->getItem()->getStyle()->setLineColor(uicontext()->lightColor());
	edge->getItem()->getStyle()->setDstShape(qan::EdgeStyle::ArrowShape::None);

	//junction->setWorkflowNode(static_cast<WorkflowGraph *>(getGraph())->workflow()->workflow()->nodeById(nodeId));

	return junction;

}

void RangeGroup::setRangeNode(SPLRangeNode node) {
	_range = node;
	if (_range) {
		setLabel(_range->name());
	}
}

WorkflowModel *RangeGroup::workflow() {
	return static_cast<WorkflowGraph *>(getGraph())->workflow();
}

int	RangeGroup::nodeId() {
	if (_range) {
		return _range->id();
	}
	return iUNDEF;
}



void WorkflowGraphNode::inNodeOutputChanged()
{

}

WorkflowModel *WorkflowGraphNode::workflow() {
	return static_cast<WorkflowGraph *>(getGraph())->workflow(); 
}

const WorkflowModel *WorkflowGraphNode::workflow() const {
	if ( getGraph())
		return static_cast<const WorkflowGraph *>(getGraph())->workflow();
	return 0;
}

void WorkflowGraphNode::setWorkflowNode(SPWorkFlowNode node) {
	_workflowNode = node;
	if (_workflowNode) {
		setLabel(_workflowNode->name());
		emit nodeIdChanged();
	}
}

void WorkflowGraphNode::setWorkflowNode(NodeId nodeid) {
	if (workflow()) {
		auto node = workflow()->workflow()->nodeById(nodeid);
		setWorkflowNode(node);
	}
}

void WorkflowGraphNode::setOutput(QVariant output) noexcept
{
	_output = output;
	emit outputChanged();
}

int WorkflowGraphNode::nodeId() const {
	if (_workflowNode) {
		return (int)_workflowNode->id();
	}
	return iUNDEF;
}

int WorkflowGraphNode::nodeId()  {
	if (_workflowNode) {
		return (int)_workflowNode->id();
	}
	return iUNDEF;
}

void WorkflowGraphNode::updateBoundingBox() {
	if (_workflowNode) {
		auto item = getItem();
		_workflowNode->box(BoundingBox(Pixel(item->x(), item->y()), Pixel(item->x() + item->width(), item->y() + item->height())));
	}
}

QQmlComponent*  OperationGraphNode::delegate(QQmlEngine& engine) noexcept
{
	static std::unique_ptr<QQmlComponent>   delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/OperationNode.qml");
	return delegate.get();
}

void OperationGraphNode::inNodeOutputChanged()
{
	WorkflowGraphNode::inNodeOutputChanged();
	qreal o{ 0. }; // For the example sake we do not deal with overflow
	setOutput(o);
}

void OperationGraphNode::operationModel(const QString& operationId) {
	auto ocatalog = uicontext()->globalOperationCatalog();
	_operation = ocatalog->operation(operationId);
	emit operationChanged();

}
Ilwis::Ui::OperationModel *OperationGraphNode::operation()
{ 
	return _operation; 
}

QQmlComponent*  ConditionTestValue::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/ConditionTestValueNode.qml");
	return delegate.get();
}

QQmlComponent*  ConditionJunction::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/JunctionNode.qml");
	return delegate.get();
}

QQmlComponent*  RangeJunction::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/RangeJunctionNode.qml");
	return delegate.get();
}

QString RangeJunction::outputType() const {
	if (_workflowNode) {
		// initial input or range input determine the type of the output of the junction;
		auto parm = _workflowNode->input(RangeNode::rpINITIALINPUT);
		QString tp = TypeHelper::type2name(parm.valueType());
		if (tp == sUNDEF) {
			 parm = _workflowNode->input(RangeNode::rpINPUT);
			 tp = TypeHelper::type2name(parm.valueType());
		}
		return tp;
	}
	return sUNDEF;
}

QQmlComponent*  RangeValue::delegate(QQmlEngine& engine) noexcept {
	static std::unique_ptr<QQmlComponent>   delegate;
	if (!delegate)
		delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/RangeValue.qml");
	return delegate.get();
}

void RangeValue::rangeId(quint32 id) {
	_rangeId = id;
}

void RangeValue::setRangeTestValues(const QString& rangeDef) {
	if (_rangeId != iUNDEF) {
		workflow()->setRangeTestValues(_rangeId, rangeDef);
		emit rangeValueChanged();
	}
}

QString RangeValue::rangeValue() const {
	if (workflow()) {
		SPWorkFlowNode node = workflow()->workflow()->nodeById(_rangeId);
		if (node) {
			std::shared_ptr<RangeNode> range = std::static_pointer_cast<RangeNode>(node);
			return range->rangeDefinition();
		}
	}
	return "";
}

int  RangeValue::rangeId() const {
	return _rangeId;
}

QQmlComponent*  DataFlowEdge::delegate(QQmlEngine& engine) noexcept
{
	static std::unique_ptr<QQmlComponent>   customEdge_delegate;
	if (!customEdge_delegate)
		customEdge_delegate = std::make_unique<QQmlComponent>(&engine, "qrc:/qml/datapanel/modeller/workflow/DataFlowEdge.qml");
	return customEdge_delegate.get();
}

qan::EdgeStyle* DataFlowEdge::style() noexcept
{
	static std::unique_ptr<qan::EdgeStyle>  customEdge_style;
	if (!customEdge_style)
		customEdge_style = std::make_unique<qan::EdgeStyle>();
	return customEdge_style.get();
}

int DataFlowEdge::toParmIndex() const {
	return _inParmIndex;
}
void DataFlowEdge::toParmIndex(int idx) {
	_inParmIndex = idx;
}
int DataFlowEdge::fromParmIndex() const {
	return _outParmIndex;
}
void DataFlowEdge::fromParmIndex(int idx) {
	_outParmIndex = idx;
}


qan::Node* WorkflowGraph::insertOperationNode(int nodeId, const QString& operationId)
{
	qan::Node* flowNode = insertNode<OperationGraphNode>(nullptr);
	OperationGraphNode *on = static_cast<OperationGraphNode *>(flowNode);
	on->operationModel(operationId);

	if (flowNode) {
		//flowNode->installBehaviour(std::make_unique<WorkflowGraphNodeBehaviour>());
		WorkflowGraphNode *wfn = static_cast<WorkflowGraphNode *>(flowNode);
		wfn->setWorkflowNode(_workflowModel->workflow()->nodeById(nodeId));
	}

	return flowNode;
}

qan::Node*  WorkflowGraph::insertOperationNode(const QVariantMap& parms) {
	qan::Node* flowNode = insertNode<OperationGraphNode>(nullptr);
	OperationGraphNode *on = static_cast<OperationGraphNode *>(flowNode);
	on->operationModel(parms["operationid"].toString());


	if (flowNode) {
		//flowNode->installBehaviour(std::make_unique<WorkflowGraphNodeBehaviour>());
		WorkflowGraphNode *wfn = static_cast<WorkflowGraphNode *>(flowNode);
		wfn->setWorkflowNode(_workflowModel->workflow()->nodeById(parms["nodeid"].toInt()));
		QRectF rect(parms["x"].toDouble(), parms["y"].toDouble(), parms["w"].toDouble(), parms["h"].toDouble());
		flowNode->getItem()->setRect(rect);
	}

	return flowNode;
}

qan::Group* WorkflowGraph::insertConditionGroup(quint32 conditionId) {
	ConditionGroup *gr = static_cast<ConditionGroup *>(qan::Graph::insertGroup<ConditionGroup>());
	auto testNode = insertNode<ConditionTestValue>(nullptr);
	testNode->getItem()->setX(5);
	testNode->getItem()->setY(5);
	testNode->getItem()->setResizable(false);
	testNode->getItem()->setDraggable(false);
	testNode->getItem()->setParentItem(gr->getItem());

	gr->setTest(static_cast<ConditionTestValue *>(testNode));
	SPCondition p = std::static_pointer_cast<WorkFlowCondition>(_workflowModel->workflow()->nodeById(conditionId));
	WorkFlowCondition::Test tst = p->test(0);
	auto node = _workflowModel->workflow()->nodeById(tst._operation->id());
	static_cast<WorkflowGraphNode *>(testNode)->setWorkflowNode(node);
	gr->setConditionNode(p);

	groupNode(gr, testNode, false);

	return gr;

}

qan::Group* WorkflowGraph::insertRangeGroup(quint32 rangeId) {
	RangeGroup *gr = static_cast<RangeGroup *>(qan::Graph::insertGroup<RangeGroup>());
	auto testNode = insertNode<RangeValue>(nullptr);
	testNode->getItem()->setX(5);
	testNode->getItem()->setY(5);
	testNode->getItem()->setResizable(false);
	testNode->getItem()->setDraggable(false);
	testNode->getItem()->setParentItem(gr->getItem());

	gr->setRangeValue(static_cast<RangeValue *>(testNode));
	SPLRangeNode p = std::static_pointer_cast<RangeNode>(_workflowModel->workflow()->nodeById(rangeId));
	gr->setRangeNode(p);
	static_cast<RangeValue *>(testNode)->rangeId(p->id());
	static_cast<RangeValue *>(testNode)->setRangeTestValues(p->rangeDefinition());

	groupNode(gr, testNode, false);

	return gr;

}

qan::Edge*  WorkflowGraph::insertDataFlowEdge(qan::Node* source, qan::Node* destination, int fromIdx, int toIdx)
{
	const auto engine = qmlEngine(this);
	if (source != nullptr && destination != nullptr && engine != nullptr) {
		auto *edge = qan::Graph::insertEdge<DataFlowEdge>(*source, destination, DataFlowEdge::delegate(*engine));
		auto *dataflow = static_cast<DataFlowEdge *>(edge);
		dataflow->toParmIndex(toIdx);
		dataflow->fromParmIndex(fromIdx);

	}
	return nullptr;
}

qan::Group* WorkflowGraph::insertRangeGroup(const QVariantMap& parms) {
	RangeGroup *rgroup = static_cast<RangeGroup *>(insertRangeGroup(parms["nodeid"].toUInt()));
	QRectF rect(parms["x"].toDouble(), parms["y"].toDouble(), parms["w"].toDouble(), parms["h"].toDouble());
	if (rgroup->getItem())
		static_cast<qan::GroupItem *>(rgroup->getItem())->setRect(rect);
	return rgroup;

}

qan::Group* WorkflowGraph::insertConditionGroup(const QVariantMap& parms) {
	RangeGroup *cgroup = static_cast<RangeGroup *>(insertConditionGroup(parms["nodeid"].toUInt()));
	QRectF rect(parms["x"].toDouble(), parms["y"].toDouble(), parms["w"].toDouble(), parms["h"].toDouble());
	if (cgroup->getItem())
		static_cast<qan::GroupItem *>(cgroup->getItem())->setRect(rect);
	return cgroup;

}

void WorkflowGraph::setWorkflow(Ilwis::Ui::WorkflowModel *fmodel) {
	_workflowModel = fmodel;
}

void WorkflowGraph::initialize() {
	qmlRegisterType< Ilwis::Ui::WorkflowGraphNode >("Ilwis.Workflow", 1, 0, "WorkflowGraphNode");
	qmlRegisterType< Ilwis::Ui::ConditionJunction >("Ilwis.Workflow", 1, 0, "ConditionJunction");
	qmlRegisterType< Ilwis::Ui::WorkflowGraph >("Ilwis.Workflow", 1, 0, "WorkflowGraph");
	qmlRegisterType< Ilwis::Ui::WorkflowView >("Ilwis.Workflow", 1, 0, "WorkflowView");
	qmlRegisterType< Ilwis::Ui::OperationGraphNode >("Ilwis.Workflow", 1, 0, "OperationGraphNode");
	qmlRegisterType< Ilwis::Ui::ConditionGroup >("Ilwis.Workflow", 1, 0, "ConditionGroup");
	qmlRegisterType< Ilwis::Ui::RangeGroup >("Ilwis.Workflow", 1, 0, "RangeGroup");
	qmlRegisterType< Ilwis::Ui::RangeJunction >("Ilwis.Workflow", 1, 0, "RangeJunction");

}

void WorkflowGraph::setFlow(int nodeFromId, int nodetoId, int inParmIndex, int outParmIndex) {
	if (_workflowModel) {
		auto nodeTo = _workflowModel->workflow()->nodeById(nodetoId);
		auto nodeFrom = _workflowModel->workflow()->nodeById(nodeFromId);
		if (nodeTo->type() == WorkFlowNode::ntOPERATION ) {
			_workflowModel->addFlow(nodeFromId, nodetoId, inParmIndex, outParmIndex);
		}
		if (nodeTo->type() == WorkFlowNode::ntJUNCTION) {
			auto nodeOwnerFrom = nodeFrom->owner();
			std::shared_ptr<JunctionNode> junction = std::static_pointer_cast<JunctionNode>(nodeTo);
			bool trueCase = nodeOwnerFrom && (nodeOwnerFrom->id() == junction->conditionId());
			_workflowModel->addFlow(nodeFromId, nodetoId, trueCase ? 1 : 2, outParmIndex);
		}
		if (nodeTo->type() == WorkFlowNode::ntRANGEJUNCTION) {
			auto nodeFrom = _workflowModel->workflow()->nodeById(nodeFromId);
			auto nodeOwnerFrom = nodeFrom->owner();
			if (nodeOwnerFrom) {
				std::shared_ptr<RangeJunctionNode> junction = std::static_pointer_cast<RangeJunctionNode>(nodeTo);
				bool initCase = nodeOwnerFrom->id() != junction->rangeId();
				_workflowModel->addJunctionFlows( nodetoId,nodeFromId, outParmIndex, initCase);
			}
			else {
				_workflowModel->addJunctionFlows(nodetoId, nodeFromId, outParmIndex, true);
			}
		}
	}
	
}

void WorkflowGraph::updateBoundingBoxes() {
	for (auto iter = this->cbegin(); iter != this->cend(); ++iter) {
		auto value = (*iter).get();
		WorkflowGraphNode *wnode = dynamic_cast<WorkflowGraphNode *>(value);
		if ( wnode)
			wnode->updateBoundingBox();
		else {
			auto gnode = dynamic_cast<RangeGroup *>(value);
			if ( gnode)
				gnode->updateBoundingBox();
			else {
				auto cnode = dynamic_cast<ConditionGroup *>(value);
				if (cnode)
					cnode->updateBoundingBox();
			}
		} 
	}
}

qan::EdgeStyle *WorkflowGraph::junctionEdgeStyle() const {
	return _junctionEdgeStyle;
}

void WorkflowGraph::removeLinksFrom(WorkflowGraphNode* node) {
	auto removeConnectEdge = [this, node](auto& edge) {
		auto edgePtr = edge.lock();
		if (edgePtr &&
			edgePtr->getItem() != nullptr &&
			(edgePtr->getItem()->getSourceItem() == node->getItem() ||
				edgePtr->getItem()->getDestinationItem() == node->getItem()))
			this->removeEdge(edgePtr.get());
	};
	std::for_each(node->get_in_edges().begin(), node->get_in_edges().end(), removeConnectEdge);
	std::for_each(node->get_out_edges().begin(), node->get_out_edges().end(), removeConnectEdge);

}

bool WorkflowGraph::containedInLinkedCondition(int sourceId, int targetId) const {
	auto sourceNode = _workflowModel->workflow()->nodeById(sourceId);
	auto targetNode = _workflowModel->workflow()->nodeById(targetId);
	if (sourceNode && targetNode) {
		if (sourceNode->type() == WorkFlowNode::ntOPERATION &&  targetNode->type() == WorkFlowNode::ntJUNCTION) {
			return  sourceNode->owner() && sourceNode->owner()->id() == targetNode->owner()->id();
		}
	}
	return false;
}
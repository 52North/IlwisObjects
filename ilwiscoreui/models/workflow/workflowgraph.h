#pragma once


// QuickQanava headers
#include "kernel.h"
#include "ilwisdata.h"
#include "QuickQanava.h"

// Qt headers
#include <QQuickPaintedItem>
namespace Ilwis {
	class Workflow;
	class WorkFlowNode;
	class WorkFlowCondition;
	class RangeNode;

	typedef std::shared_ptr<WorkFlowNode> SPWorkFlowNode;
	typedef std::shared_ptr<WorkFlowCondition> SPCondition;
	typedef IlwisData<Workflow> IWorkflow;
	typedef std::shared_ptr<RangeNode> SPLRangeNode;

	namespace Ui {
		class WorkflowModel;
		class OperationModel;
		class ConditionTestValue;
		class ConditionJunction;
		class RangeJunction;
		class RangeValue;
		class WorkflowGraph;
		class WorkflowGraphNode;

		class  WorkflowGraphNodeBehaviour : public qan::NodeBehaviour
		{
			Q_OBJECT
		public:
			explicit WorkflowGraphNodeBehaviour(QObject* parent = nullptr) : qan::NodeBehaviour{ "WorkflowGraphNodeBehaviour", parent } { /* Nil */ }
		protected:
			virtual void    inNodeInserted(qan::Node& inNode, qan::Edge& edge) noexcept override;
			virtual void    inNodeRemoved(qan::Node& inNode, qan::Edge& edge) noexcept override;
		};

	

		class WorkflowGraphNode : public qan::Node
		{
			Q_OBJECT
		public:
			enum class Type {
				Operation,
				Range,
				Condition,
				ConditionJunction,
				RangeJunction,
				ConditionTestValue,
				RangeValue
			};
			Q_ENUM(Type)

				explicit WorkflowGraphNode(QQuickItem* parent = nullptr) : WorkflowGraphNode(Type::Operation, parent) {}
			explicit WorkflowGraphNode(Type type, QQuickItem* parent = nullptr) :
				qan::Node{ parent }, _type{ type } { /* Nil */ }
			virtual ~WorkflowGraphNode() { /* Nil */ }

			WorkflowGraphNode(const WorkflowGraphNode&) = delete;
			WorkflowGraphNode& operator=(const WorkflowGraphNode&) = delete;
			WorkflowGraphNode(WorkflowGraphNode&&) = delete;
			WorkflowGraphNode& operator=(WorkflowGraphNode&&) = delete;

			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;

		public:
			Q_PROPERTY(Type type READ getType CONSTANT FINAL)
				Q_PROPERTY(QVariant output READ getOutput WRITE setOutput NOTIFY outputChanged)
				Q_PROPERTY(int nodeId READ nodeId NOTIFY nodeIdChanged)

				Q_INVOKABLE void setWorkflowNode(quint64 node);
			Q_INVOKABLE void updateBoundingBox();

			void setWorkflowNode(SPWorkFlowNode node);

			inline  Type    getType() const noexcept { return _type; }
			int				nodeId();
			int				nodeId() const;
			WorkflowModel*	workflow();
			const WorkflowModel *workflow() const;

		public slots:
			virtual void    inNodeOutputChanged();
		signals:
			void            outputChanged();
			void			nodeIdChanged();

		protected:
			QVariant        _output;
			Type            _type{ WorkflowGraphNode::Type::Operation };
			SPWorkFlowNode	_workflowNode;


			inline QVariant getOutput() const noexcept { return _output; }
			void            setOutput(QVariant output) noexcept;


		};

		class ConditionGroup : public qan::Group {
			Q_OBJECT
		public:
			Q_INVOKABLE void addNode(qan::NodeItem *nodeItem);
			Q_INVOKABLE Ilwis::Ui::ConditionJunction* addJunction();
			Q_PROPERTY(int nodeId READ nodeId NOTIFY nodeIdChanged)
			Q_PROPERTY(Ilwis::Ui::WorkflowGraphNode::Type type READ getType CONSTANT FINAL)

			WorkflowGraphNode::Type getType() const { return WorkflowGraphNode::Type::Condition; }

			explicit ConditionGroup(QObject* parent = nullptr) : qan::Group(parent) {}
			virtual ~ConditionGroup() override { /* Nil */ }

			void setTest(ConditionTestValue *tst);

			static QQmlComponent* delegate(QQmlEngine &engine) noexcept;
			static qan::Style*    style() noexcept;
			WorkflowModel *workflow();
			void setConditionNode(SPCondition node);
			void updateBoundingBox();
		signals:
			void			nodeIdChanged();
		private:
			int				nodeId();
			ConditionGroup(const ConditionGroup &) = delete;

			ConditionTestValue *_testNode = 0;
			SPCondition _condition;
		};

		class RangeGroup : public qan::Group {
			Q_OBJECT
		public:
			Q_INVOKABLE void addNode(qan::NodeItem *nodeItem);
			Q_INVOKABLE void addNode2GroupNode(qan::Node *node);
			Q_INVOKABLE Ilwis::Ui::RangeJunction* addJunction();
			Q_PROPERTY(int nodeId READ nodeId NOTIFY nodeIdChanged)
			Q_PROPERTY(Ilwis::Ui::WorkflowGraphNode::Type type READ getType CONSTANT FINAL)
			Q_PROPERTY(Ilwis::Ui::RangeValue *rangeValue READ rangeValue CONSTANT FINAL);

			WorkflowGraphNode::Type getType() const { return WorkflowGraphNode::Type::Range;}

			explicit RangeGroup(QObject* parent = nullptr) : qan::Group{ parent } {}
			virtual ~RangeGroup() override { /* Nil */ }

			static QQmlComponent* delegate(QQmlEngine &engine) noexcept;
			static qan::Style*    style() noexcept;
			WorkflowModel *workflow();
			void setRangeNode(SPLRangeNode node);
			void setRangeValue(RangeValue *tst);
			Ilwis::Ui::RangeValue *rangeValue() const { return _rangeValue; }
			void updateBoundingBox();

		signals:
			void			nodeIdChanged();
			void			rangeIdChanged();
		private:
			int				nodeId();
			RangeGroup(const ConditionGroup &) = delete;
			SPLRangeNode _range;
			RangeValue *_rangeValue;
		};

	

		class ConditionJunction : public WorkflowGraphNode {
			Q_OBJECT
		public:
			ConditionJunction() : WorkflowGraphNode{ WorkflowGraphNode::Type::ConditionJunction } {  }
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;

		};

		class ConditionTestValue : public WorkflowGraphNode {
			Q_OBJECT
		public:
			ConditionTestValue() : WorkflowGraphNode{ WorkflowGraphNode::Type::ConditionTestValue } {  }
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;

	
		};

		class RangeValue : public WorkflowGraphNode {
			Q_OBJECT
		public:
			RangeValue() : WorkflowGraphNode{ WorkflowGraphNode::Type::RangeValue } {  }

			Q_PROPERTY(QString rangeValue READ rangeValue NOTIFY rangeValueChanged)

			Q_INVOKABLE void setRangeTestValues(const QString& rangeDef);
			Q_PROPERTY(int rangeId READ rangeId NOTIFY rangeIdChanged)
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;

			void rangeId(quint32 id);
			int rangeId() const;

		signals:
			void rangeIdChanged();
			void rangeValueChanged();

		private:
			QString rangeValue() const;
			quint32 _rangeId = iUNDEF;


		};

		class RangeJunction : public WorkflowGraphNode {
			Q_OBJECT
		public:
			RangeJunction() : WorkflowGraphNode{ WorkflowGraphNode::Type::RangeJunction } {  }
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;

			Q_INVOKABLE QString outputType() const;

		};

		class OperationGraphNode : public WorkflowGraphNode
		{
			Q_OBJECT
		public:
			Q_PROPERTY(Ilwis::Ui::OperationModel *operation READ operation CONSTANT)

			OperationGraphNode() : WorkflowGraphNode{ WorkflowGraphNode::Type::Operation } {  }
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;
			void operationModel(const QString& operationId);

		signals:
			void                operationChanged();

		protected slots:
			void                inNodeOutputChanged();

		private:
			Ilwis::Ui::OperationModel *_operation = 0;

			Ilwis::Ui::OperationModel *operation() { return _operation; }
		};

		class DataFlowEdge : public qan::Edge
		{
			Q_OBJECT
		public:
			explicit DataFlowEdge() : qan::Edge{} { }
			virtual ~DataFlowEdge() { /* Nil */ }
			DataFlowEdge(const DataFlowEdge&) = delete;

			Q_PROPERTY(int toParmIndex READ toParmIndex WRITE toParmIndex NOTIFY toParmChanged)
			Q_PROPERTY(int fromParmIndex READ fromParmIndex WRITE fromParmIndex NOTIFY fromParmChanged)

		public:
			static  QQmlComponent*      delegate(QQmlEngine& engine) noexcept;
			static  qan::EdgeStyle*     style() noexcept;

			int toParmIndex() const;
			void toParmIndex(int idx);
			int fromParmIndex() const;
			void fromParmIndex(int idx);

		signals:
			void toParmChanged();
			void fromParmChanged();

		private:

			int _inParmIndex = iUNDEF;
			int _outParmIndex = iUNDEF;
		};

		QML_DECLARE_TYPE(DataFlowEdge)


		class WorkflowGraph : public qan::Graph
		{
			Q_OBJECT
		public:
			explicit WorkflowGraph(QQuickItem* parent = nullptr) noexcept : qan::Graph(parent) { _junctionEdgeStyle = new qan::EdgeStyle("junctionedge", parent); }
			static void initialize();
		public:
			Q_INVOKABLE qan::Node*  insertOperationNode(int nodeId, const QString& operationId);       // FlowNode::Type could not be used from QML, Qt 5.10 bug???
			Q_INVOKABLE qan::Node*  insertOperationNode(const QVariantMap& parms);
			Q_INVOKABLE qan::Group*  insertConditionGroup(quint32 conditionId);
			Q_INVOKABLE qan::Group* insertRangeGroup(quint32 conditionId);
			Q_INVOKABLE qan::Group* insertRangeGroup(const QVariantMap& parms);
			Q_INVOKABLE qan::Edge* insertDataFlowEdge(qan::Node* source, qan::Node* destination, int from, int to);
			Q_INVOKABLE void setWorkflow(Ilwis::Ui::WorkflowModel *workflow);
			Q_INVOKABLE void setFlow(int nodeFromId, int nodetoId, int inParmIndex, int outParmIndex);
			Q_INVOKABLE Ilwis::Ui::WorkflowModel *workflow() const { return _workflowModel; }
			Q_INVOKABLE void updateBoundingBoxes();
			Q_INVOKABLE void removeLinksFrom(WorkflowGraphNode* source);
			qan::EdgeStyle *junctionEdgeStyle() const;

		private:
			WorkflowModel *_workflowModel;
			qan::EdgeStyle *_junctionEdgeStyle;
		};

		class WorkflowView : public qan::GraphView {
		public:
			explicit WorkflowView(QQuickItem* parent = nullptr) : GraphView(parent) {};

			WorkflowView(const WorkflowView&) = delete;
		};

	}


}



QML_DECLARE_TYPE(Ilwis::Ui::WorkflowGraphNode)
QML_DECLARE_TYPE(Ilwis::Ui::WorkflowGraph)
//QML_DECLARE_TYPE(Ilwis::Ui::WorkflowView);
Q_DECLARE_METATYPE(Ilwis::Ui::WorkflowGraphNode::Type)
QML_DECLARE_TYPE(Ilwis::Ui::ConditionGroup)
//Q_DECLARE_METATYPE(qan::OperationNode::Operation)
